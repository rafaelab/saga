import math
import os
import sys

from crpropa import *
import saga


N = 3

convLength = 8.57251e+24 # m
convDensity = 2.49651e-27 # kg/m3
convMagneticField = 1.11868099322e-09 # Tesla

convMagneticField *= 1e1 # scaling to get physical values


fn = '/home/rafaelab/Work/Simulations/LSS/output_00083.sql'
magF = saga.MagneticField(saga.AMRgrid(fn, 10))
bField = AMRMagneticField(magF, convLength, convDensity, convMagneticField)
boxOrigin = Vector3d(0, 0, 0)
boxSize = Vector3d(convLength, convLength, convLength) 

#observer
obsPosition = boxSize * 0.5
obsSize = convLength / 2.
obs = Observer()
obs.add(ObserverLargeSphere(obsPosition, obsSize))
#obs.add(ObserverOutput3D(OutputName))

# output
t = TextOutput('test.txt')
t.printHeader()
obs.onDetection(t)

# source and observer parameters
source = Source()
source.add(SourcePosition(obsPosition))
source.add(SourceIsotropicEmission())
source.add(SourceParticleType(nucleusId(1, 1)))
source.add(SourceEnergy(1e20 * eV))


# module setup
m = ModuleList()
m.add(DeflectionCK(bField, 1e-2, 100 * kpc, 1 * Mpc))
m.add(PeriodicBox(boxOrigin, boxSize))
m.add(obs)

# run
m.showModules()
m.setShowProgress(True)
m.run(source, N, True)


