# import from make location
import importlib.util
spec = importlib.util.spec_from_file_location(
    "PyeSpinn", "build/src/PyeSpinn.cpython-37m-x86_64-linux-gnu.so")
eSpinn = importlib.util.module_from_spec(spec)
spec.loader.exec_module(eSpinn)


# test neuron
neuron = eSpinn.Neuron(1, eSpinn.INPUT)
print(neuron)
print("neuron has id #", neuron.id, sep='')
print("neuron is in", neuron.getLayer())
neuron.layer = eSpinn.HIDDEN
print("neuron now is in", neuron.layer)

print()
sensor = eSpinn.Sensor(1)
print(sensor)

print()
izhi = eSpinn.IzhiNeuron(2, eSpinn.HIDDEN)
print(izhi)

print()
sigm = eSpinn.SigmNeuron(3, eSpinn.OUTPUT)
print(sigm)

print()
sigm2 = sigm # the same object
print(sigm2)
sigm2.setID(4)
print(sigm)


print()
# test connection
conn = eSpinn.Connection(1)
print(conn)

conn2 = eSpinn.Connection(2, sensor, izhi)
print(conn2)

sconn = eSpinn.SpikeConnection(3, izhi, sigm)
print(sconn)

print()

# test network
netbase = eSpinn.NetworkBase(1)
print(netbase)

sigmnet = eSpinn.SigmNetwork(1, 3, 1, 1)
print(sigmnet)
inp = [0.1, 0.1, 0.3]
sigmnet.load_inputs(inp)
outp = sigmnet.run()
print("net out is", outp)

print()

# test organism
orgbase = eSpinn.OrganismBase(2, 1)
print(orgbase)

# test org<net>
org = eSpinn.IzhiOrganism(2, 3, 2, 1, 3)
print(org)

org1 = eSpinn.HybLinOrganism(2, 2, 2, 1)
print(org1)

# test species
species = eSpinn.Species(2)
print(species)


# test population
pop = eSpinn.Population(org, 2)
pop.init()
print(pop)
orgs = pop.orgs
print(orgs)
champ = pop.get_champ_org()
print(champ)
spe = pop.species
print(spe)
pop.archive("./asset/archive/pop1.arch")

print()

# test injector
inj = eSpinn.Injector(2)
inj.setNormFactors(-2.0, 2.0, 0)
inj.setNormFactors(-3.0, 3.0, 1)
inj.load_data(0, 1.0)
inj.load_data(1, -2.1)
norm_data = inj.get_data_set()
print(norm_data)
sigmnet.load_inputs(norm_data)
print(sigmnet.run())

for o in orgs:
    net = o.net()
    net.load_inputs(norm_data)
    print(net.run())

print()

