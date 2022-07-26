# Copyright (C) 2017-2019 Huanneng Qiu.
# This file is proprietary and confidential.
# Unauthorized copying of this file via any medium is strictly prohibited.


"""
Spiking Bird Learning
"""

import random
import importlib.util
import sys
import subprocess
import argparse
parser = argparse.ArgumentParser(
    usage='%(prog)s [-h]/[-opt]',
    allow_abbrev=False,
    description='Evolving flappy bird neural network brain')
group = parser.add_mutually_exclusive_group(required=True)
group.add_argument('-t', '--train', action='store_true', help='training mode')
group.add_argument(
    '-e', '--eval', type=int, metavar='',
    help="evaluation mode, followed by a 'gen' that specifies the loading file")
args = parser.parse_args()

import pygame
# import from make location
spec = importlib.util.spec_from_file_location(
    "PyeSpinn", "build/src/PyeSpinn.cpython-37m-x86_64-linux-gnu.so")
eSpinn = importlib.util.module_from_spec(spec)
spec.loader.exec_module(eSpinn)

spec = importlib.util.spec_from_file_location(
    "flappybird", "game/flappybird.py")
flappy = importlib.util.module_from_spec(spec)
spec.loader.exec_module(flappy)


# class NNBird
class NNBird(flappy.Bird):
    """ AI Bird, flap controlled by neural net """
    def __repr__(self):
        return 'NN Bird '+str(self.org)
    def __init__(self, coord, baseY, image, org):
        super().__init__(coord, baseY, image)
        self.org = org

    def load_states(self, inputs):
        """ Load input states """
        net = self.org.net()
        net.load_inputs(inputs)

    def update(self):
        """ Update bird with net output """
        flapped = self.org.net().run()[0]
        if flapped > .0:
            self.flap()
        super().update()


def create_nnbirds(g, orgs):
    """ create num AI birds """
    nnbirds = []
    baseY = g.base.Y
    num = len(orgs)
    for i in range(num):
        rand_shift = random.randint(-10, 10)
        midY = int((g.HEIGHT - g.IMAGES['bird'][0].get_height()) / 2)
        birdXY = [int(g.WIDTH * .2), midY + rand_shift]
        bird = NNBird(birdXY, baseY, g.IMAGES, orgs[i])
        nnbirds.append(bird)
    return nnbirds


def refresh(game, birds, dist, gen, alive):
    """ Refresh game window """
    # background, pipes & base
    game.SCREEN.blit(game.IMAGES['background'], (0, 0))
    for p in game.pipes:
        p.draw(game.SCREEN)
    game.base.update()
    game.base.draw(game.SCREEN)

    # stat
    Dist = stat_font.render('Dist: ' + str(dist), 1, color)
    Gen = stat_font.render('Gen: ' + str(gen), 1, color)
    Alive = stat_font.render('Alive: ' + str(alive), 1, color)
    game.SCREEN.blit(Dist, (game.WIDTH - Dist.get_width() - 10, 10))
    game.SCREEN.blit(Gen, (10, 10))
    game.SCREEN.blit(Alive, (10, 40))

    # birds
    for b in birds:
        b.flapWing()
        b.draw(game.SCREEN)

    pygame.display.update()
    game.FPSCLOCK.tick(game.FPS)


def eval(game, birds, inj, gen):
    """ Evaluate brids in FlappyBird Game simutaniously """
    # reset stat
    dist = 0
    # birds & pipes
    for b in birds:
        b.softreset()
    game.pipes.clear()
    pipe1 = game.createPipe(game.WIDTH + 200)
    pipe2 = game.createPipe(game.WIDTH + 200 + pipe1.width + game.PIPEGAP_H)
    game.pipes = [pipe1, pipe2]
    while True:
        for event in pygame.event.get():
            if event.type == pygame.QUIT or \
            (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE):
                pygame.quit()
                sys.exit()


        # check for crash status
        birds_alive = []
        for b in birds:
            if flappy.checkCrash(b, game.pipes):
                b.org.fit = dist
            else:
                birds_alive.append(b)
        birds = birds_alive
        alive = len(birds)
        if not alive:
            return dist
        dist += 1
        if dist > 10000:
            for b in birds:
                b.org.fit = dist
            return dist

        # update score
        birdMidPos = birds[0].X + birds[0].width / 2
        for pipe in game.pipes:
            pipeMidPos = pipe.x + pipe.width / 2
            if pipeMidPos <= birdMidPos < pipeMidPos + game.SPEED:
                game.score += 1

        # update birds
        # input states normalized & loaded to each bird
        # err_x: horizontal dist btw bird (left edge) & pipe (right edge)
        # err_y: height dist btw bird (top edge) & pipe gap (middle)

        # first decide which pipe
        i = 0
        for p in game.pipes:
            if p.x + p.width > birds[0].X:
                break
            i += 1
        pipe_ahead = game.pipes[i]

        # Xs of birds are identical, so err_x is calculated only once
        err_x = pipe_ahead.x + pipe_ahead.width - birds[0].X
        # err_x /= 10.0 # reduce the err_x impact to increase evolution speed
        inj.load_data(0, err_x) # normalization
        for b in birds:
            err_y = b.y - (pipe_ahead.lowerY - game.PIPEGAP / 2)
            inj.load_data(1, err_y) # normalization
            norm_inputs = inj.get_data_set()
            # load inputs & update
            b.load_states(norm_inputs)
            b.update()
        # update pipes
        game.updatePipes()
        # draw sprites
        refresh(game, birds, dist, gen, alive)


# init game
pygame.init()
game = flappy.Flappy(pygame)
stat_font = pygame.font.SysFont('arial', 25)
color = (255, 255, 255)


def train():
    """ Training process """

    print()
    print('Training birds ...')

    # input & output num
    inp_num = 3 # err_x, err_y & bias
    outp_num = 1

    # create a population
    # org = eSpinn.LinrOrganism(0, inp_num, 0, outp_num)
    org = eSpinn.LifOrganism(0, inp_num, 0, outp_num)
    pop = eSpinn.Population(org, 50)
    pop.init()

    # init inject encoder
    # inj is used to normalize input states:
    # err_x: horizontal dist btw bird (left edge) & pipe (right edge)
    #   err_x = pipe.x + pipe.width - bird.x
    # err_y: height dist btw bird (top edge) & pipe gap (middle)
    #   err_y = bird.y - (pipe.lowery - gap/2)
    inj = eSpinn.Injector(inp_num-1)
    MAX_ERRX = game.PIPEGAP_H + game.IMAGES['pipe'][0].get_width()
    MAX_ERRY = game.bird.yBOTTOM - int(0.2*game.base.Y + game.PIPEGAP/2)
    MIN_ERRY = game.bird.yTOP - int(0.8*game.base.Y - game.PIPEGAP/2)
    inj.setNormFactors(0, MAX_ERRX, 0)
    inj.setNormFactors(MIN_ERRY, MAX_ERRY, 1) # [-297, 250]
    inj.archive('./asset/archive/inj.arch')

    fit_file = './asset/data/fit'
    fit_logger = eSpinn.Logger()

    # gen
    gen = 1
    gens = 50 # gen threshold

    while gen <= gens:
        print('--------------------------------------------------')
        print(pop)

        birds = create_nnbirds(game, pop.orgs)
        # evaluate birds
        dist = eval(game, birds, inj, gen)
        # save dist to file
        fit_logger.append_to_file(dist, fit_file)
        # if solved
        if dist > 10000:
            pop.set_solved()
        pop.archive('./asset/archive/pop' + str(gen) + '.arch')
        champ = pop.get_champ_org()
        champ.save('./asset/archive/champ_org.arch')
        if pop.issolved():
            print('Dist reaches 10000')
            break

        # evolve
        pop.epoch(gen)
        gen += 1

    # backup pop config files
    subprocess.call('./asset/bkp_arch.sh')


def evaluate(gen):
    """ Evaluation process """

    print()
    print('Evaluating champion bird of gen', gen, '...')

    # load pop from file
    pop = eSpinn.Population()
    pop.load('./asset/archive/pop' + str(gen) + '.arch')
    # load inject encoder used to normalize input states
    inj = eSpinn.createInjector('./asset/archive/inj.arch')

    bird = create_nnbirds(game, [pop.get_champ_org()])
    # evaluate bird
    dist = eval(game, bird, inj, gen)
    print('Champion bird dist =', dist)


if __name__ == '__main__':
    """ Training or Evaluation """
    if args.train:
        train()
    else:
        evaluate(args.eval)
