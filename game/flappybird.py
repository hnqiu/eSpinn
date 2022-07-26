# Copyright (C) 2017-2019 Huanneng Qiu.
# Licensed under the Apache-2.0 license. See LICENSE for details.


"""
Flappy Bird game implementation
"""

import random
import sys
from itertools import cycle
import pygame


class Flappy:
    """
    Flappy Bird game
    """
    # window settings
    FPS = 30
    WIDTH = 288
    HEIGHT = 512
    FPSCLOCK, SCREEN = {}, {}
    # game definitions
    SPEED = 4 # moving speed
    PIPEGAP = 100 # gap between upper and lower part of pipe
    PIPEGAP_H = 100 # horizontal gap between adjacent pipes
    # image and sound dicts
    IMAGES, SOUNDS = {}, {}

    # objects
    bird, pipes, base = {}, {}, {}

    # welcome message
    msgXY = {}
    # score
    score = 0

    def __init__(self, g):
        """initialization of flappy bird"""
        # init window
        self.FPSCLOCK = g.time.Clock()
        self.SCREEN = g.display.set_mode((self.WIDTH, self.HEIGHT))
        g.display.set_caption('Flappy Bird')

        # image and sound dicts
        self.IMAGES, self.SOUNDS = asset('game/assets/')
        # welcome message
        self.msgXY = [int((self.WIDTH - self.IMAGES['message'].get_width()) /2),
                      int(self.HEIGHT * 0.12)]

        # construct objects
        baseXY = [0, int(self.HEIGHT * .79)]
        birdXY = [int(self.WIDTH * .2),
                  int((self.HEIGHT - self.IMAGES['bird'][0].get_height()) / 2)]
        self.base = Base(baseXY, self.SPEED, self.IMAGES)
        self.bird = Bird(birdXY, baseXY[1], self.IMAGES)

    def main(self):
        """ main loop """
        while True:
            self.showWelcome()
            self.play()
            self.gameOver()

    def showWelcome(self):
        """ Shows welcome screen animation of flappy bird """
        # reset
        self.bird.reset()
        self.pipes.clear()
        self.score = 0

        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT or \
                (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE):
                    pygame.quit()
                    sys.exit()
                if event.type == pygame.KEYDOWN and \
                (event.key == pygame.K_SPACE or event.key == pygame.K_UP):
                    self.SOUNDS['wing'].play()
                    return

            # bird oscillate
            self.bird.shm()
            # draw sprites
            self.refresh('welc')

    def play(self):
        """ Play the game """
        self.bird.softreset()
        pipe1 = self.createPipe(self.WIDTH + 100)
        pipe2 = self.createPipe(self.WIDTH + 100 + pipe1.width + self.PIPEGAP_H)
        self.pipes = [pipe1, pipe2]
        pause = False

        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT or \
                (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE):
                    pygame.quit()
                    sys.exit()
                if event.type == pygame.KEYDOWN:
                    if event.key == pygame.K_SPACE or event.key == pygame.K_UP:
                        self.SOUNDS['wing'].play()
                        self.bird.flap()
                    if event.key == pygame.K_p:
                        # pause or continue
                        pause = not pause

            # check for crash status
            isCrashed = checkCrash(self.bird, self.pipes)
            if isCrashed:
                return

            # update score
            birdMidPos = self.bird.X + self.bird.width / 2
            for pipe in self.pipes:
                pipeMidPos = pipe.x + pipe.width / 2
                if pipeMidPos <= birdMidPos < pipeMidPos + self.SPEED:
                    self.SOUNDS['point'].play()
                    self.score += 1

            if pause:
                # draw sprites
                self.refresh('play_pause')
            else:
                # update bird
                self.bird.update()
                # update pipes
                self.updatePipes()
                # draw sprites
                self.refresh('play_cont')

    def gameOver(self):
        """ Show gameover screen """
        # play hit and die sounds
        self.SOUNDS['hit'].play()
        self.SOUNDS['die'].play()

        while True:
            for event in pygame.event.get():
                if event.type == pygame.QUIT or \
                (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE):
                    pygame.quit()
                    sys.exit()
                if event.type == pygame.KEYDOWN and \
                (event.key == pygame.K_SPACE or event.key == pygame.K_UP):
                    return

            # bird falling down
            self.bird.update()
            # draw sprites
            self.refresh('over')


    def createPipe(self, pos_x):
        """ returns a pair of randomly generated pipes (upper & lower) """
        #               |   |
        #  upper pipe:  |   |
        #               |   |
        #               |   |
        #       gapY -> *----
        #
        #
        #               -----
        #               |   |
        #  lower pipe:  |   |
        #               |   |
        #               |   |
        # gap between upper and lower pipe
        # gapY in [0.2baseY, 0.8baseY-PIPEGAP)
        gapY = random.randrange(0, int(self.base.Y * 0.6 - self.PIPEGAP))
        gapY += int(self.base.Y * 0.2)
        pipeHeight = self.IMAGES['pipe'][0].get_height()
        pipeX = pos_x

        pipe_pair = {
            'x': pipeX,
            'upper': gapY - pipeHeight, # upper y
            'lower': gapY + self.PIPEGAP # lower y
        }
        return Pipe(pipe_pair, self.SPEED, self.IMAGES)

    def updatePipes(self):
        """ move pipes, create new pipes & remove old pipes """
        # move pipes to left
        for p in self.pipes:
            p.move()
        # add new pipes
        if 0 < self.pipes[0].x < self.SPEED+1: # x==4
            diff = self.pipes[1].x - self.pipes[0].x
            p = self.createPipe(self.pipes[1].x + diff)
            self.pipes.append(p)
        # remove old pipes
        if self.pipes[0].x < -self.pipes[0].width:
            self.pipes.pop(0)


    def refresh(self, flag):
        """ refresh game window """
        # background, pipes & base
        self.SCREEN.blit(self.IMAGES['background'], (0, 0))
        for p in self.pipes:
            p.draw(self.SCREEN)
        if flag in ('welc', 'play_cont'):
            self.base.update()
        self.base.draw(self.SCREEN)

        # messages & score
        if flag == 'welc':
            self.SCREEN.blit(self.IMAGES['message'], self.msgXY)
        elif flag == 'over':
            self.SCREEN.blit(self.IMAGES['gameover'], (50, 180))
        if flag in ('play_cont', 'play_pause', 'over'):
            self.showScore()

        # bird
        self.bird.flapWing()
        if flag == 'over':
            self.bird.draw(self.SCREEN, 'dead')
        else:
            self.bird.draw(self.SCREEN)

        pygame.display.update()
        self.FPSCLOCK.tick(self.FPS)


    def showScore(self):
        """ displays score in center of screen """
        scoreDigits = [int(x) for x in list(str(self.score))]
        totalWidth = 0 # total width of all numbers to be printed

        for digit in scoreDigits:
            totalWidth += self.IMAGES['numbers'][digit].get_width()

        Xoffset = (self.WIDTH - totalWidth) / 2

        for digit in scoreDigits:
            self.SCREEN.blit( self.IMAGES['numbers'][digit],
                              (Xoffset, self.HEIGHT * 0.1) )
            Xoffset += self.IMAGES['numbers'][digit].get_width()


class Base:
    """
    Definition of Base
    """
    def __init__(self, coord, speed, image):
        self.x, self.Y = coord[0], coord[1]
        self.speed = speed
        self.IMG = image['base']
        # amount base can maximum shift to left
        self.SHIFT = image['base'].get_width() - image['background'].get_width()

    def update(self):
        """ update base position """
        self.x = -((-self.x + self.speed) % self.SHIFT)

    def draw(self, obj):
        """ draw base on screen """
        obj.blit(self.IMG, (self.x, self.Y))


class Bird:
    """
    Definition of Bird
    """
    # bird wing mode
    wingIndex = 0 # index to load bird mode
    wingMode = cycle([0, 1, 2, 1]) # wing mode
    loopIter = 0
    # up-down simple harmonic motion on welcome screen
    birdShm = {'val': 0, 'index': 1}

    # bird states
    X        =   0      # constant
    y        =   0
    INITY    =   0      # init y on welcome, constant
    yTOP     = -24      # top y, constant
    yBOTTOM  = 100      # bottom y, constant, set in __init__
    velY     =   0
    velMAXY  =  10      # max descend velocity, const
    velMINY  =  -8      # min ascend vel, const
    ACCY     =   1      # constant
    rotation =   0
    rate     =   3      # rotation rate
    flapVel  =  -9      # flap speed, constant
    flapped  =  False

    # image size
    width, height = {}, {}

    def __init__(self, coord, baseY, image):
        self.X, self.y, self.INITY = coord[0], coord[1], coord[1]
        self.IMG = image['bird'] # 3 wing modes
        self.width = image['bird'][0].get_width()
        self.height = image['bird'][0].get_height()
        self.yBOTTOM = baseY - self.height
        self.MASK = (
            # hitmask for different bird wing modes
            getHitmask(image['bird'][0]),
            getHitmask(image['bird'][1]),
            getHitmask(image['bird'][2])
        )

    def shm(self):
        """oscillates the value of birdShm['val'] between 8 and -8"""
        if abs(self.birdShm['val']) == 8:
            self.birdShm['index'] *= -1

        if self.birdShm['index'] == 1:
            self.birdShm['val'] += 1
        else:
            self.birdShm['val'] -= 1
        self.y += self.birdShm['val']

    def softreset(self):
        """ reset bird states """
        self.loopIter = 0
        self.wingIndex = 0
        self.velY = -9
        self.rotation = 30

    def reset(self):
        """ reset bird states """
        self.softreset()
        self.rotation = 0
        self.y = self.INITY
        self.birdShm = {'val': 0, 'index': 1}

    def flapWing(self):
        """ wing animation """
        self.loopIter += 1
        if self.loopIter == 10: # flap wing every 10 frames
            self.wingIndex = next(self.wingMode)
            self.loopIter = 0

    def flap(self):
        """ bird flapped """
        self.flapped = True


    def update(self):
        """ update bird vel, pos & rotation """
        if self.flapped:
            self.velY = self.flapVel
            self.rotation = 30
            self.flapped = False
        elif self.velY < self.velMAXY:
            self.velY += self.ACCY

        self.y += self.velY
        if self.y < self.yTOP:
            self.y = self.yTOP
        elif self.y > self.yBOTTOM:
            self.y = self.yBOTTOM

        # do not rotate if it is on the ground
        if not self.touchground():
            if self.rotation > -70:
                self.rotation -= self.rate

    def draw(self, obj, isdead = ''):
        """ draw bird on screen """
        if isdead == 'dead':
            # use the midflap image
            rotImg = pygame.transform.rotate(self.IMG[1],self.rotation)
        else:
            rotImg = pygame.transform.rotate(self.IMG[self.wingIndex],
                                             self.rotation)
        obj.blit(rotImg, (self.X, self.y))

    def touchground(self):
        """ check if on ground """
        return self.y >= self.yBOTTOM


class Pipe:
    """
    Definition of Pipe
    An instance contains a pair of upper and lower pipes
    """
    def __init__(self, p, speed, image):
        self.x = p['x']
        self.upperY, self.lowerY = p['upper'], p['lower']
        self.speed = speed
        self.IMG = image['pipe'] # upper & lower pipes
        self.width = image['pipe'][0].get_width()
        self.height = image['pipe'][0].get_height()
        self.MASK = (
            # hitmask for pipes
            getHitmask(image['pipe'][0]), # upper
            getHitmask(image['pipe'][1]) # lower
        )

    def move(self):
        """ move pipe """
        self.x -= self.speed

    def draw(self, obj):
        """ draw upper and lower pipes """
        obj.blit(self.IMG[0], (self.x, self.upperY))
        obj.blit(self.IMG[1], (self.x, self.lowerY))


def getHitmask(image):
    """returns a hitmask using an image's alpha."""
    mask = []
    for x in range(image.get_width()):
        mask.append([])
        for y in range(image.get_height()):
            mask[x].append(bool(image.get_at((x,y))[3]))
    return mask


def checkCrash(bird, pipes):
    """ Returns True if bird collders with base or pipes. """
    # if crashes into ground
    if bird.touchground():
        return True

    # if collide with pipes
    birdRect = pygame.Rect(bird.X, bird.y, bird.width, bird.height)
    birdMask = bird.MASK[bird.wingIndex]
    for p in pipes:
        uRect = pygame.Rect(p.x, p.upperY, p.width, p.height)
        lRect = pygame.Rect(p.x, p.lowerY, p.width, p.height)
        uMask = p.MASK[0]
        lMask = p.MASK[1]
        uCollide = pixelCollision(birdRect, uRect, birdMask, uMask)
        lCollide = pixelCollision(birdRect, lRect, birdMask, lMask)

        if uCollide or lCollide:
            return True

    return False


def pixelCollision(rect1, rect2, hitmask1, hitmask2):
    """ Checks if two objects collide and not just their rects """
    rect = rect1.clip(rect2)

    if rect.width == 0 or rect.height == 0:
        return False

    x1, y1 = rect.x - rect1.x, rect.y - rect1.y
    x2, y2 = rect.x - rect2.x, rect.y - rect2.y

    for x in range(rect.width):
        for y in range(rect.height):
            if hitmask1[x1+x][y1+y] and hitmask2[x2+x][y2+y]:
                return True
    return False


def asset(asset_dir):
    """ load assets: image and sound """
    IMAGES, SOUNDS = {}, {}

    # list of all possible players (tuple of 3 positions of flap)
    BIRDS_LIST = (
        # red bird
        (
            asset_dir + 'sprites/redbird-upflap.png',
            asset_dir + 'sprites/redbird-midflap.png',
            asset_dir + 'sprites/redbird-downflap.png',
        ),
        # blue bird
        (
            asset_dir + 'sprites/bluebird-upflap.png',
            asset_dir + 'sprites/bluebird-midflap.png',
            asset_dir + 'sprites/bluebird-downflap.png',
        ),
        # yellow bird
        (
            asset_dir + 'sprites/yellowbird-upflap.png',
            asset_dir + 'sprites/yellowbird-midflap.png',
            asset_dir + 'sprites/yellowbird-downflap.png',
        ),
    )

    # list of backgrounds
    BG_LIST = (
        asset_dir + 'sprites/background-day.png',
        asset_dir + 'sprites/background-night.png',
    )

    # list of pipes
    PIPES_LIST = (
        asset_dir + 'sprites/pipe-green.png',
        asset_dir + 'sprites/pipe-red.png',
    )

    # numbers sprites for score display
    IMAGES['numbers'] = (
        pygame.image.load(asset_dir + 'sprites/0.png').convert_alpha(),
        pygame.image.load(asset_dir + 'sprites/1.png').convert_alpha(),
        pygame.image.load(asset_dir + 'sprites/2.png').convert_alpha(),
        pygame.image.load(asset_dir + 'sprites/3.png').convert_alpha(),
        pygame.image.load(asset_dir + 'sprites/4.png').convert_alpha(),
        pygame.image.load(asset_dir + 'sprites/5.png').convert_alpha(),
        pygame.image.load(asset_dir + 'sprites/6.png').convert_alpha(),
        pygame.image.load(asset_dir + 'sprites/7.png').convert_alpha(),
        pygame.image.load(asset_dir + 'sprites/8.png').convert_alpha(),
        pygame.image.load(asset_dir + 'sprites/9.png').convert_alpha()
    )

    # game over sprite
    IMAGES['gameover'] = pygame.image.load(
        asset_dir + 'sprites/gameover.png').convert_alpha()
    # message sprite for welcome screen
    IMAGES['message'] = pygame.image.load(
        asset_dir + 'sprites/message.png').convert_alpha()
    # base (ground) sprite
    IMAGES['base'] = pygame.image.load(
        asset_dir + 'sprites/base.png').convert_alpha()
    # background sprites
    bgindex = random.randint(0, len(BG_LIST) - 1)
    IMAGES['background'] = pygame.image.load(BG_LIST[bgindex]).convert()

    # bird sprites
    birdindex = random.randint(0, len(BIRDS_LIST) - 1)
    IMAGES['bird'] = (
        pygame.image.load(BIRDS_LIST[birdindex][0]).convert_alpha(),
        pygame.image.load(BIRDS_LIST[birdindex][1]).convert_alpha(),
        pygame.image.load(BIRDS_LIST[birdindex][2]).convert_alpha(),
    )

    # pipe sprites
    pipeindex = random.randint(0, len(PIPES_LIST) - 1)
    IMAGES['pipe'] = (
        pygame.transform.flip(
            pygame.image.load(PIPES_LIST[pipeindex]).convert_alpha(), False, True),
        pygame.image.load(PIPES_LIST[pipeindex]).convert_alpha(),
    )

    # sounds
    SOUNDS['die']    = pygame.mixer.Sound(asset_dir + 'audio/die.ogg')
    SOUNDS['hit']    = pygame.mixer.Sound(asset_dir + 'audio/hit.ogg')
    SOUNDS['point']  = pygame.mixer.Sound(asset_dir + 'audio/point.ogg')
    SOUNDS['swoosh'] = pygame.mixer.Sound(asset_dir + 'audio/swoosh.ogg')
    SOUNDS['wing']   = pygame.mixer.Sound(asset_dir + 'audio/wing.ogg')

    return IMAGES, SOUNDS


if __name__ == '__main__':
    pygame.init()
    game = Flappy(pygame)
    game.main()
