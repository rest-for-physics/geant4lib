# The geant4 library
[![DOI](https://zenodo.org/badge/324291710.svg)](http://doi.org/10.5281/zenodo.4528985)
[![pipeline status](https://gitlab.cern.ch/rest-for-physics/geant4lib/badges/master/pipeline.svg)](https://gitlab.cern.ch/rest-for-physics/geant4lib/-/commits/master)
[![website](https://img.shields.io/badge/user-guide-E8B6FF.svg)](https://rest-for-physics.github.io)
[![api](https://img.shields.io/badge/user-API-FFCA78.svg)](https://sultan.unizar.es/rest/)
[![forum](https://img.shields.io/badge/user-forum-AAFF90.svg)](https://rest-forum.unizar.es/)

This library is used to store and analyse the events generated in a Geant4 simulation, it defines and stores the particle generator and simulation conditions, such as the details of the physics list used during the Monte Carlo.

Please have a look to this library [contribution guide](CONTRIBUTING.md) before pushing changes to this repository.

## Lindhard for contained, stopped recoils

`TRestGeant4QuenchingProcess` has an opt-in gas model that evaluates the existing
Lindhard formula once at a recoil track's initial kinetic energy `E0`. The same
factor `q(E0, A, Z)` multiplies every local deposit on that track. Subdividing the
same deposits therefore preserves the total visible energy, apart from floating
point rounding. The original `lindhard` model still evaluates the factor per hit
using its hadronic target information; `auto`, Birks, and existing configurations
keep their historical behavior.

```xml
<TRestGeant4QuenchingProcess>
    <parameter name="applyToHitEnergies" value="true"/>
    <volume name="gasVolume" model="lindhard-stopped-recoil"/>
</TRestGeant4QuenchingProcess>
```

Use this option on unmodified deposited hit energies, before converting them to
electrons or applying any other ionization-yield correction. `A` and `Z` come from
the recoil's ground-state nuclear PDG code in the simulation metadata; a proton
uses `A=Z=1`. Hadronic target isotope fields are not used. Electromagnetic tracks,
neutrons, and other non-nuclear particles are unchanged by this model. A neutron
event can consequently contain quenched recoil tracks and unquenched
electromagnetic secondary tracks. This is an approximation, not a measured yield
curve for any particular gas mixture.

The selected recoil must have a saved `Init` state, remain in one selected
physical volume, end with zero saved kinetic energy, and have no explicit children
(neither saved secondary IDs nor tracks with that parent ID). Each saved kinetic
energy loss must match the corresponding local deposit, and the sum must match
`E0`. The numerical tolerance is `8 * epsilon(float) * E0`, because REST stores
deposited and kinetic energies as floats. Missing metadata, unknown or excited
nuclear identity, escaping tracks, secondary-producing tracks, or energy that
does not close are rejected with an exception identifying the event and track;
there is no fallback to another recoil model.

Simulation metadata must have `storeTracks=true`, `registerEmptyTracks=true`,
`removeUnwantedTracks=false`, no kill volumes, and storage probability one for all
geometry volumes. Complete volume storage prevents an unrecorded exit and
re-entry from looking contained. For example, retain the full geometry when
configuring the simulation:

```xml
<parameter name="storeTracks" value="true"/>
<parameter name="registerEmptyTracks" value="true"/>
<detector activateAllVolumes="true">
    <removeUnwantedTracks enabled="false"/>
    <volume name="gasVolume" sensitive="true" chance="1"/>
</detector>
```

REST does not store separate NIEL values, terminal track status, or material phase
in these histories. The checks establish eligibility from the recorded steps;
the user selects the gas volumes. Filtered historical files may be unsupported.
With `applyToHitEnergies=false`, only event energy accounting is converted; keep
it `true` when the next process generates charge from the hit energies. An
attempt to convert already quenched hit energies again fails the kinetic-loss
closure check.

The synthetic `ProcessEvent` tests in
[`test/src/Geant4QuenchingProcesses.cxx`](test/src/Geant4QuenchingProcesses.cxx)
cover deposit subdivision, recoil identity, unchanged electromagnetic/neutron
hits, event energy accounting, historical Lindhard, and the unsupported-history
checks. They require no transport simulation or external ROOT data file.
