/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
/// TRestGeant4Metadata is the main class used to interface with *restG4* (a REST
/// based Geant4 code distributed with REST) used to launch Geant4 based
/// simulations, and store later on the simulation conditions as metadata
/// information inside the output generated file. The simulations produced by
/// *restG4* will write to disk the event data generated as a TRestGeant4Event type.
/// The tutorials page includes a tutorial describing on detail how to launch a
/// restG4 simulation and retrieve basic simulation results.
///
/// There are few helper classes that aid to organize and access the information
/// that TRestGeant4Metadata contains, TRestGeant4Particle, TRestGeant4BiasingVolume,
/// TRestGeant4ParticleSource.
///
/// After installation of restG4, some basic working examples can be found at
/// `$REST_PATH/examples/`. We use this class to store restG4 rml parameters like
/// particle type, energy, direction, gdml definition, etc. In addition, we have
/// a class TRestGeant4PhysicsLists in parallel to define the necessary physics
/// processes, EM, hadronic, etc, that will be active in our Geant4 simulation.
///
/// In general terms, an RML file to be used with restG4 must define the
/// following sections, and structures.
///
/// \code
///
/// // We must, as usual, define the location where the REST output files will
/// // be stored
/// <globals>
///    <parameter name="mainDataPath" value="${REST_DATAPATH}" />
///    <searchPath value="/path/to/geometry" />
/// </globals>
///
/// // A TRestRun section to define few run parameters with a general run
/// // description.
/// <TRestRun>
///     ...
/// </TRestRun>
///
/// //A TRestGeant4Metadata section defining few parameters, generator, and storage.
/// <TRestGeant4Metadata>
///     ...
/// </TRestGeant4Metadata>
///
/// //A TRestGeant4PhysicsLists section def-inning the physics processes active.
/// <TRestGeant4PhysicsLists>
///     ...
/// </TRestGeant4PhysicsLists>
///
/// \endcode
///
/// \note Wherever 3 dots (`...`) are provided means a redundant code format, or
/// that additional fields might be required.
///
/// \note The runTag inside the TRestRun class will be overwritten by the name
/// of TRestGeant4Metadata section defined in the RML.
///
/// This page describes in detail the different parameters, particle generator
/// types, storage, and other features implemented in restG4, that can be
/// defined inside the section TRestGeant4Metadata. The description of other
/// required sections, as TRestRun or TRestGeant4PhysicsLists, will be found in their
/// respective class documentation.
///
/// We can sub-divide the information provided through TRestGeant4Metadata in
/// different parts,
///
/// 1. the main parameters related to the simulation conditions, such as number
/// of events to be simulated, random seed, or GDML geometry definitions
/// file,
///
/// 2. the definition of the primary particle generator, using the `<generator>`
/// section, to define which particles will be generated, from where they will
/// be generated, and which energies and direction distributions they will
/// follow,
///
/// 3. the definition of what event hits will be written to disk, using the
/// `<storage>` section,
///
/// 4. and the (optional) definition of biasing volumes to simulate particle
/// transmission through extended detector shieldings, using the `<biasing>`
/// section.
///
/// ## 1. Basic simulation parameters
///
/// The following list presents the common parameters that must be defined
/// inside TRestGeant4Metadata section to define few simulation conditions in
/// *restG4*, as important as the detector geometry, or the number of events to
/// be simulated.
///
/// * **nEvents**: The number of primary particles to be generated. The number
/// of registered events might differ from *nEvents* due to storage definitions.
/// The number of events registered could be even larger than the number of
/// primaries, as for example when launching full decay chain simulations, where
/// different isotope decays are stored in different events.
///
/// [GDML]: https://gdml.web.cern.ch/GDML/
/// * **gdmlFile**: The path and name of the main GDML file. Both relative and
/// absolute path is supported. In principle, the
/// user has full freedom to create any detector setup geometry using a
/// [GDML][GDML] description.
///
/// \warning The only requirement is that the gas logical volume (implemented
/// in a single physical volume on the geometry) must be named `gasVolume`.
///
///
/// * **subEventTimeDelay**: This parameter defines the event time window. If a
/// Geant4 energy deposit takes place after this time, those hits will be
/// registered as a fully independent event. Therefore, the total number of
/// registered events could be higher than the initial number of events.
///
/// * **saveAllEvents**: If active, this parameter will mark all the volumes
/// of the geometry as active, and it will ignore the energy range definition
/// in the *storage* section. Any Geant4 simulated track from any event or
/// subevent will be registered even if no energy deposition have been produced.
/// In future, this parameter would be better implemented inside `<storage`
/// definition.
///
/// The following example illustrates the definition of the common simulation
/// parameters.
///
/// \code
///		<parameter name="nEvents" value="100" />
///		<parameter name="gdmlFile" value="/path/to/mySetupTemplate.gdml"/>
///		<parameter name="maxTargetStepSize" value="200" units="um" />
///		<parameter name="subEventTimeDelay" value="100" units="us" />
/// \endcode
///
/// ## 2. The primary particle generator section
///
/// The generator section describes from where we will launch our primary
/// particles, which particles we will launch, the energy of these particles,
/// their angular distribution and other required functions needed to cover
/// different physics cases.
///
/// The complete structure of the primary particle generator follows this
/// scheme.
///
/// \code
/// <generator type="generatorType" ... >
///
///     <source use="geant4" particle="particleName" ... >
///         <angularDist type="angularDistribution" />
///         <energyDist type="energyDistribution" energy="energyValues" units="MeV" />
///     </source>
///
///     <source particle="particleName" ... >
///         ...
///     </source>
///
///     // Add any number of sources.
///
/// </generator>
/// \endcode
///
/// ### The generator type definition
///
/// The different generator types determine the spatial origin of the primaries
/// that will be generated.
///
/// We can define any number of primary particles (sources) that will be used
/// to construct an initial event. All primary particles that build an
/// event will have a common position(*). The spatial origin of these sources, or
/// primaries, is specified in the definition of the generator. We have 4 major types
/// of generator, namely:
///
/// * **custom**: Uses positions specified in the generator file of the particle
/// source. (NOT IMPLEMENTED)
///
/// * **volume**: Launch the events from random positions inside a volume of certain
/// shape.
///
/// * **surface**: Launch the events from random positions upon the surface of certain
/// shape.
///
/// * **point**: Launch the events from a single point of given position.
/// \code
///     // We launch particles at a fix position at the XY origin and z=-15cm
///     <generator type="point" position="(0,0,-150)" units="mm" >
/// \endcode
///
/// Together with the generator type, we need to define the shape, size and position.
/// Both size and position are in TVector3 form. The following shapes are supported:
///
/// * **gdml**: Bound the generator to a certain gdml component. No need to define size
/// and position. Needs another "from" parameter. If the "from" parameter is defined,
/// we can omit `shape="gdml"` parameter.
///  \code
///     // We launch particles from random positions inside the vessel
///     // volume defined in our GDML geometry
///     <generator type="volume" from="vessel" > ... </generator>
/// \endcode
///
/// * **box**: Bound the generator to a virtual box area. "position" defines the center of the
/// box, while the three elements of "size" defines respectively x, y, z length of the box.
///  \code
///     <generator type="volume" shape="box" size="(10,20,20)" position="(0,0,5)" > ... </generator>
/// \endcode
///
/// * **cylinder**: Bound the generator to a virtual cylinder area. "position" defines the
/// center of the cylinder, while the three elements of "size" defines respectively
/// radius, length, nothing of the cylinder.
///
/// * **sphere**: Bound the generator to a virtual sphere area. "position" defines the
/// center of the sphere, while the three elements of "size" defines respectively
/// radius, nothing, nothing of the sphere. In future we may implement ellipsoid
/// support for the sphere and uses all three elements.
///
/// * **circle**: Bound the generator to a virtual circle area. "position" defines the
/// center of the circle, while the three elements of "size" defines respectively
/// radius, nothing, nothing of the circle. "circle" shape works only for "surface"
/// generator type. The initial direction of the circle is in parallel to x-y plane.
/// \code
///     // We launch particles from random positions on a virtual circle
///     <generator type="surface" shape="circle" size="(10,0,0)" position="(0,0,0)" > ... </generator>
/// \endcode
///
/// * **wall**: Bound the generator to a virtual rectangle area. "position" defines the
/// center of the rectangle, while the three elements of "size" defines respectively
/// x, y, nothing length the rectangle. "rectangle" shape works only for "surface"
/// generator type. The initial direction of the rectangle is in parallel to x-y plane.
///
/// Rotation of the virtual body defined previously is also supported. We need to define
/// parameter "rotationAxis" and "rotationAngle" to do this job. The TVector3 parameter
/// "rotationAxis" is a virtual axis passing through the center of the virtual body,
/// with its three elements representing its direction. "rotationAngle" defines the
/// rotation angle in the view of the axis direction. It is in radian unit.
/// \code
///     // We launch particles from random positions on a virtual wall leaned to 45 deg
///     <generator type="surface" shape="wall" size="(10,10,0)" position="(0,0,0)"
///     rotationAxis="(1,0,0)" rotationAngle="0.78539816" > ... </generator>
/// \endcode
///
/// By default all the primaries will be launched homogeneously from the volume or
/// the surface. We can define additional density function to customize the particle
/// origin distribution. The function is a TF3 function with x, y, z the absolute
/// position(position of gdml frame instead of generator frame) of the particle in
/// unit mm. The returned value should be in range 0-1, indicating the relative probability
/// in this position. For example, we simulate some radio isotopes placed on a chip
/// with doping which follows exponential density distribution near the surface:
/// \code
///     <generator type="volume" shape="box" size="(10,10,1)mm" position="(0,0,0.5)"
///     densityFunc="exp(-z/0.01)" > ... </generator>
/// \endcode
///
/// ### The source definition
///
/// Three type of source definitions can be used to generate primary particles
/// in our simulation. The different types are introduced through the parameter `use`.
///
/// * A pre-generated decay0 file,
/// \code
///     // 1. generator file
///     <source use="Xe136bb0n.dat" />
/// \endcode
///
/// * the in-simulation decay0 generator package
/// \code
///    // 2. decay0 package
///    <source use="decay0" particle="Xe136" decayMode="0vbb" daughterLevel="3" />
/// \endcode
///
/// * and the traditional geant4 particle description integrated in REST.
/// \code
///    // 3. geant4 internal
///    <source use="geant4" particle="Na22" excitedLevel="0.0" fullChain="on">
///		    <angularDist type="isotropic" />
///			<energyDist type="mono" energy="0.0" units="MeV" />
///	   </source>
/// \endcode
///
/// [leptons]:
/// http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/TrackingAndPhysics/AllResources/TrackingAndPhysics/particleList.src/leptons/index.html
/// [ions]:
/// http://geant4-userdoc.web.cern.ch/geant4-userdoc/UsersGuides/ForApplicationDeveloper/html/TrackingAndPhysics/AllResources/TrackingAndPhysics/particleList.src/ions/index.html
///
/// For the third case, which is the default if `use` field is not defined, we
/// can define any pre-defined particle name existing in *Geant4* in the `particle`
/// field, as [leptons][leptons] or [ions][ions]. Additionally, we can define
/// any radioactive isotope by using the element name symbol and the number of
/// nucleons (i.e. Rn222, Co60, U238 ). For the radioactive decays we can also
/// define an additional field `fullchain="on/off"`. This parameter specifies if
/// we want to simulate the full radiative chain or a single decay to the next
/// de-excited isotope.
///
/// In summary we can use the following options inside the source definition:
///
/// * **particle="G4_Particle"**: A particle name understood by Geant4 (i.e. e-,
/// e+, gamma, etc).
/// \code
///     // We launch gammas
///     <source particle="gamma">
/// \endcode
///
/// * **particle="Xn" fullchain="off"**: A radioactive isotope. Where X is the
/// element name and n the number of nucleons (I.e. Rn222, Xe137, Co60, etc).
/// \code
///     // We launch a single chain Rn222 decay
///     <source particle="Rn222" fullChain="off" >
/// \endcode
///
/// \note For ions, we can also define its electric charge (signed integer) and
/// excited
/// level value in eV, by using the additional fields excitedLevel and charge.
///
/// * **fromFile="Xe136bb0n.dat"**: The particles, energies, and direction are
/// fully described by an input file. `<energyDistribution>` and
/// `<angularDistribution>` source specifiers have no effect here. Everything
/// will be defined through the generator file. The file `Xe136bb0n.dat` is a
/// pre-generated `Decay0` file that can be found at `REST_PATH/data/generator`,
/// together with other generator examples.
/// \code
///     // We launch pre-generated 136Xe NLDBD events
///     <source fromFile="Xe136bb0n.dat">
///         // The energyDist and angularDist
///         // definitions here will be just
///         // ignored with this type of source
///     </source>
/// \endcode
///
/// #### The source energy distribution
///
/// A source, or particle, associated kinetic energy is defined by using.
/// \code
/// <energyDist type="energyDistType" ... />
/// \endcode
///
/// There are different energy distribution types we can use to define the
/// energy of the primary particles. Depending on the type of energy
/// distribution we will be required to add additional parameters. The types we
/// can use in REST are the following types.
///
/// * **mono**: All the particles from this source will be launched with the
/// same energy for each simulated event. It requires to define the parameter
/// `energy="E"`, where `E` is the kinetic energy of the particle in keV.
/// \code
///     // A mono energetic particle with 10keV.
///     <energyDist type="mono" energy="10" units="keV" />
/// \endcode
///
/// * **flat**: All the particles from this source will be launched in a
/// specified energy range using a uniform energy distribution. It requires to
/// define the parameter `range="(Ei,Ef)"`, where `Ei` is the minimum energy and
/// `Ef` is the maximum energy (in keV). \code
///     // A random uniform generation between 1 and 10 keV
///     <energyDist type="flat" range="(1,10)" units="keV" />
/// \endcode
///
/// * **log**: All the particles from this source will be launched in a
/// specified energy range using a logarithmic energy distribution. It requires to
/// define the parameter `range="(Ei,Ef)"`, where `Ei` is the minimum energy and
/// `Ef` is the maximum energy (in keV). `Ei` needs to be > 0. \code
///     // A random logarithmic generation between 1 and 10 keV
///     <energyDist type="log" range="(1,10)" units="keV" />
/// \endcode
///
/// * **TH1D**: It will use a TH1D histogram from a ROOT file with a user
/// defined spectrum. It requires to define the parameters
/// `file="mySpectrum.root"` `spctName="histName"` and `range="(Ei,Ef)"`. The
/// ROOT file should contain a TH1D histogram with name `histName`. Only the
/// region of the spectrum inside the range `Ei-Ef` will be considered. If `range`
/// is not specified inside the RML, the full TH1D range definition will be used.
/// The energy units of the spectrum must be specified in the x-axis label of the
/// TH1D object. If no units are given in the x-label, the default units
/// considered will be keV.
/// \code
///     // A TH1D input spectrum to produce underground muons in the range
///     // between 150 and 400 GeV
///     <energyDist type="TH1D" file="Muons.root" spctName="LSCMuon"
///                 range="(150,400)" units="GeV" />
/// \endcode
///
/// #### The source angular distribution
///
/// The momentum direction of a particle is specified by using.
/// \code
/// <angularDist type="angularDistType" ... />
/// \endcode
///
/// We can use different types of angular distributions to define the
/// initial direction of the particle. We can choose between the following
/// types inside REST.
///
/// * **flux**: It defines a fixed direction for the particle momentum. We
/// must specify the additional parameter `direction="(pX,pY,pZ)"` as the
/// cartesian components of the momentum direction (not required to be
/// normalized).
/// \code
///     // Particles will be launched in the positive y-axis direction.
///     <angularDist type="flux" direction="(0,1,0)" />
/// \endcode
///
/// * **isotropic**: The momentum direction of each particle will be
/// generated homogeneously. In case of *virtualSphere* and *virtualBox*
/// only the particles travelling towards the inside of the virtual
/// volume will be considered.
/// \code
///     // Particles will be launched without preferred direction.
///     <angularDist type="isotropic" />
/// \endcode
///
/// * **backtoback**: The source momentum direction will be opposite to
/// the direction of the previous source. If it is the first source the
/// angular distribution type will be re-defined to isotropic.
/// \code
///     // Particles will be launched without preferred direction.
///     <angularDist type="backtoback" />
/// \endcode
///
/// * **TH1D** : It will use a TH1D histogram from a ROOT file with a
/// user defined angular distribution. It requires to define the
/// additional parameters as `file="mySpectrum.root"` and
/// `spctName="histName"`. The file we give should be stored in
/// `"data/distributions/"` and contain a TH1D histogram with
/// name "histName".
/// \code
///     // A TH1D input angular distribution used for cosmic rays
///     <angularDist type="TH1D" file="CosmicAngles.root" spctName="Theta2" />
/// \endcode
///
/// ## 3. The storage section definition
///
/// The information we store in the ROOT file can be defined using the
/// storage section. The storage section is defined as follows
///
///  \code
///  <storage sensitiveVolume="gas">
///
///     <parameter name="energyRange" value="(0,5)" units="MeV" />
///
///      <activeVolume name="gas" chance="1" />
///      // Add as many active volumes as needed
///
///  </storage>
///  \endcode
///
/// The storage section defines the `sensitiveVolume`, and the active
/// volumes where data will be stored.
///
/// The sensitive, or active, volumes can be any physical volume defined on
/// the GDML geometry. If an event did not produce an energy deposit in the
/// sensitiveVolume, the event will not be stored at all. Therefore, the
/// sensitive volume will serve as a trigger volume to decide when an event
/// should be stored. For the moment we can only define a single sensitive
/// volume, but it might be desirable to introduce boolean operations with
/// different geometry volumes.
///
/// We can define the energy range we are interested in by defining the
/// parameter energyRange. The event will be written to disk only if the
/// total energy deposited in all the active volumes of the detector is
/// in the Ei-Ef range.
///
/// \code
/// <parameter name="energyRange" value="(Ei,Ef)" />
/// \endcode
///
/// This energy range implies that only events that produced an energy
/// deposit at the sensitive volume, and produced a total energy
/// deposit between Ei and Ef, integrated to all the active volumes, will
/// be stored.
///
/// We should define inside the `<storage>` definition all the physical
/// volumes where we want hits to be stored using `<activeVolume>`
/// definition.
///
/// \code
/// <activeVolume name="gas" chance="1" maxTargetStepSize="1mm"/>
/// \endcode
///
/// * **maxTargetStepSize**: This is the maximum integration step size allowed
/// to Geant4 when approximating the interaction of particles in a medium. Lower
/// values will provide a more accurate description of the particle tracks, but
/// will also require an additional computation time.
/// \note The parameter **maxTargetStepSize** has only effect on the target
/// volume, that is the physical volume corresponding to the logical volume
/// named `gasVolume` in the geometry.
///
///
/// In general, we will always want to store all the hits in the gas volume
/// of the TPC. But the gas being the sensitive volume does not mean that
/// hits on the gas volume will be stored. This volume should be activated
/// for storage even if we defined it as the sensitive volume. The user
/// may decide to do not store the hits in the gas volume. For example, we
/// may want to track events that deposit something on the gas volume,
/// through the hits in another volume, but not be interested to study the
/// hits inside the gas for a particular case.
///
/// Each active volume defines a chance parameter. This parameter gives the
/// probability to store the hits of an event in a particular volume. For
/// example,
///
/// \code
///      <activeVolume name="gas" chance="1" />
///      <activeVolume name="vessel" chance="0.1" />
/// \endcode
///
/// will store all the hits produced in the gas, and 10% of the events will
/// include information of the hits in the vessel. This may be used as a
/// control volume to allow further study of events depositing energy in the
/// vessel, but saving some space in disk in case we do not need to use all
/// the event population.
///
/// \note If we do not specify any *activeVolume*, then all volumes found
/// in the GDML geometry will be marked as *activeVolume*. If the *chance*
/// parameter is not given, the chance will be 1 by default.
///
/// On top of that, each activeVolume may define a user limit on the
/// maximum step size of particles in that particular volume specifying the
/// *maxStepSize* parameter.
///
/// \code
///      <activeVolume name="gas" chance="1" maxStepSize="2mm" />
///      <activeVolume name="vessel" chance="0.1" maxStepSize="1cm" />
/// \endcode
///
/// Smaller values will provide a higher amount of detail, but it will require
/// additional computational time and storage.
///
/// \note If *maxStepSize* is not defined, the default value will be 0, and
/// user limits will not be applied for that particular active volume.
///
/// We are still allowed to define a default step size for all the active
/// volumes, using:
///
/// \code
/// 	<storage sensitiveVolume="gas" maxStepSize="1mm" />
/// \endcode
///
///
/// ## 4. The biasing volumes section (optional)
///
/// The REST Geant4 toolkit (*restG4*) implements a particular biasing
/// technique to simulate external radiation contributions in extensive
/// shieldings. The technique consists in spatially placing biasing volumes
/// between the initial particle generator and the detector (or sensitive
/// volume).
///
/// The largest biasing volume must be fully contained inside the original
/// event generator. And the smaller biasing volume should fully contain
/// all the volumes where hits will be stored. The different biasing
/// volumes must be fully contained one in each other (in other words,
/// the biasing volumes should not overlap).
///
/// In practice, *restG4* will run a total of N+1 *Geant4* simulations,
/// being N the number of biasing volumes defined. The first simulation
/// will just count the number of particles of the specified type traversing
/// the biasing volume, registering also their energy and angular
/// distribution. In the second simulation, the number of particles that
/// traversed the biasing volume will be multiplied by a factor, that is
/// defined independently at each biasing volume, using the recorded energy
/// spectrum from the previous simulation. This will continue until the
/// last smaller biasing volume is reached. Only in the last simulation,
/// when events are launched from the smaller biasing volume, hits will be
/// stored.
///
/// This biasing technique has been already tested using a virtualBox
/// generator and virtualBox biasing volumes, leading to compatible results
/// if compared to non biased simulations. When using this technique it is
/// recommended that you first do a small scale version of your simulation
/// to compare biased and non-biased results. If both results (from biased
/// and non-biased) agree then you will be more confident you will not
/// obtain misleading results.
///
/// In order to obtain the best results using this technique you should try
/// to keep the same number of events reaching each biasing volume. First,
/// some optimization is required. You will need to empirically test the
/// number of events reaching each biasing volume, and adjust the factors
/// according to these results. In order to have reasonable good results
/// you should assure to have at least 10,000 events reaching each of the
/// biasing volumes. If some energies are very unlikely, and not enough
/// statistics exist at the biasing volume the error will be much higher.
/// This should be solved in a future implementation by using weighting
/// factors.
///
/// The biasing technique needs to be activated using the following scheme
///
/// \code
/// <biasing value="on" type="virtualBox,virtualSphere" >
///
///     <biasingVolume particle="particleName" size="L" position="(X,Y,Z)"
///     factor="F" energyRange="(Ei,Ef)" />
///     // Add as many biasing volumes as desired
///
/// </biasing>
/// \endcode
///
/// \note The biasing is optional, and only required in exceptional
/// circumstances, most of the times will be disabled by using `value="off"`.
///
/// We can use two spatial generators to define the biasing volumes (virtualBox
/// and virtualSphere). It is recommended to use the same type as the original
/// generator. But it is left to the user to make his choice.
///
/// \warning The volumes should be placed in increasing order. From smaller to
/// larger volume.
///
///  A biasing volume is defined as follows
///
/// \code
/// <biasingVolume particle="particleName" size="L" position="(X,Y,Z)"
/// factor="F" energyRange="(Ei,Ef)" /> \endcode
///
/// The particle name may be any particle name defined in Geant4. The biasing
/// particle we define is the only particle that will be transferred to the next
/// biasing volume. Therefore, this biasing method is used to study the
/// transmission of a given particle gamma, neutron, etc through a shielding).
/// The size defines the side of the virtualBox or the radius of the
/// virtualSphere, its position `X`, `Y`, `Z` and a factor `F` that is the
/// multiplication factor to be used in the next run (In other words, the number
/// of particles launched from a biasing volume will be the number of particles
/// that reached the volume times `F`). Only the particles in the specified
/// energyRange `(Ei,Ef)` will be considered in the transmission to the next
/// biasing volume.
///
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2015-July: First concept and implementation of TRestG4 classes.
/// \author     Javier Galan
///
/// \class TRestGeant4Metadata
///
/// <hr>
///

#include "TRestGeant4Metadata.h"

#include <TGeoManager.h>
#include <TRandom.h>
#include <TRestRun.h>

#include "TRestGDMLParser.h"

using namespace std;

namespace g4_metadata_parameters {
string CleanString(string s) {
    // transform the string to lowercase
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    // this is a temporary fix, TH1D name comparison is being done elsewhere and giving error
    if (s == "th1d") {
        s = "TH1D";
    }
    return s;
}

map<string, generator_types> generator_types_map = {
    {CleanString("custom"), generator_types::CUSTOM},
    {CleanString("volume"), generator_types::VOLUME},
    {CleanString("surface"), generator_types::SURFACE},
    {CleanString("point"), generator_types::POINT},
};

std::map<string, generator_shapes> generator_shapes_map = {
    {CleanString("gdml"), generator_shapes::GDML},     {CleanString("wall"), generator_shapes::WALL},
    {CleanString("circle"), generator_shapes::CIRCLE}, {CleanString("box"), generator_shapes::BOX},
    {CleanString("sphere"), generator_shapes::SPHERE}, {CleanString("cylinder"), generator_shapes::CYLINDER},
};

map<string, energy_dist_types> energy_dist_types_map = {
    {CleanString("TH1D"), energy_dist_types::TH1D},
    {CleanString("mono"), energy_dist_types::MONO},
    {CleanString("flat"), energy_dist_types::FLAT},
    {CleanString("log"), energy_dist_types::LOG},
};

map<string, angular_dist_types> angular_dist_types_map = {
    {CleanString("TH1D"), angular_dist_types::TH1D},
    {CleanString("isotropic"), angular_dist_types::ISOTROPIC},
    {CleanString("flux"), angular_dist_types::FLUX},
    {CleanString("backtoback"), angular_dist_types::BACK_TO_BACK},
};
}  // namespace g4_metadata_parameters

ClassImp(TRestGeant4Metadata);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestGeant4Metadata::TRestGeant4Metadata() : TRestMetadata() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param configFilename A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// corresponding TRestGeant4Metadata section inside the RML.
///
TRestGeant4Metadata::TRestGeant4Metadata(const char* configFilename, const string& name)
    : TRestMetadata(configFilename) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    PrintMetadata();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestGeant4Metadata::~TRestGeant4Metadata() { RemoveParticleSources(); }

///////////////////////////////////////////////
/// \brief Initialization of TRestGeant4Metadata members
///
void TRestGeant4Metadata::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fChance.clear();
    fActiveVolumes.clear();
    fBiasingVolumes.clear();

    fNBiasingVolumes = 0;

    RemoveParticleSources();

    fSensitiveVolume = "gas";

    fEnergyRangeStored.Set(0, 1.E20);
}

///////////////////////////////////////////////
/// \brief Initialization of TRestGeant4Metadata members through a RML file
///
void TRestGeant4Metadata::InitFromConfigFile() {
    this->Initialize();

    fMagneticField = Get3DVectorParameterWithUnits("magneticField", TVector3(0, 0, 0));

    // Initialize the metadata members from a configfile
    fGdmlFilename = GetParameter("gdmlFile");
    if (fGdmlFilename == PARAMETER_NOT_FOUND_STR) {
        fGdmlFilename = GetParameter("gdml_file");  // old name
    }
    if (fGdmlFilename == PARAMETER_NOT_FOUND_STR) {
        cout << "\"gdmlFile\" parameter is not defined!" << endl;
        exit(1);
    }

    fGeometryPath = GetParameter("geometryPath", "");

    string seedString = GetParameter("seed", "0");
    if (ToUpper(seedString) == "RANDOM" || ToUpper(seedString) == "RAND" || ToUpper(seedString) == "AUTO" ||
        seedString == "0") {
        auto dd = new double();
        fSeed = (uintptr_t)dd + (uintptr_t)this;
        delete dd;
    } else {
        fSeed = (Long_t)StringToInteger(seedString);
    }
    gRandom->SetSeed(fSeed);

    // if "gdmlFile" is purely a file (without any path) and "geometryPath" is
    // defined, we recombine them together
    if ((((string)fGdmlFilename).find_first_not_of("./~") == 0 || ((string)fGdmlFilename).find("/") == -1) &&
        fGeometryPath != "") {
        if (fGeometryPath[fGeometryPath.Length() - 1] == '/') {
            fGdmlFilename = fGeometryPath + GetParameter("gdmlFile");
        } else {
            fGdmlFilename = fGeometryPath + "/" + GetParameter("gdmlFile");
        }
    }

    Double_t defaultTime = 1. / REST_Units::s;
    fSubEventTimeDelay = GetDblParameterWithUnits("subEventTimeDelay", defaultTime);

    auto nEventsString = GetParameter("nEvents");
    if (nEventsString == PARAMETER_NOT_FOUND_STR) {
        nEventsString = GetParameter("Nevents");  // old name
    }
    if (nEventsString == PARAMETER_NOT_FOUND_STR) {
        cout << "\"nEvents\" parameter is not defined!" << endl;
        exit(1);
    }
    fNEvents = StringToInteger(nEventsString);

    fSaveAllEvents = ToUpper(GetParameter("saveAllEvents", "false")) == "TRUE" ||
                     ToUpper(GetParameter("saveAllEvents", "off")) == "ON";

    fPrintProgress = ToUpper(GetParameter("printProgress", "false")) == "TRUE" ||
                     ToUpper(GetParameter("printProgress", "off")) == "ON";

    fRegisterEmptyTracks = ToUpper(GetParameter("registerEmptyTracks", "false")) == "TRUE" ||
                           ToUpper(GetParameter("registerEmptyTracks", "off")) == "ON";

    ReadGenerator();

    ReadStorage();

    ReadBiasing();

    fMaxTargetStepSize = GetDblParameterWithUnits("maxTargetStepSize", -1);
    if (fMaxTargetStepSize > 0) {
        cout << " " << endl;
        RESTWarning << "IMPORTANT: *maxTargetStepSize* parameter is now obsolete!" << RESTendl;
        RESTWarning << "The sensitive volume will not define any integration step limit" << RESTendl;
        cout << " " << endl;
        RESTWarning << "In order to avoid this warning REMOVE the *maxTargetStepSize* definition,"
                    << RESTendl;
        RESTWarning << "and replace it by the following statement at the <storage> section" << RESTendl;
        cout << " " << endl;
        RESTWarning << "<activeVolume name=\"" << this->GetSensitiveVolume() << "\" maxStepSize=\""
                    << fMaxTargetStepSize << "mm\" />" << RESTendl;
        cout << " " << endl;
        RESTInfo << "Now, any active volume is allowed to define a maxStepSize" << RESTendl;
        cout << " " << endl;
        RESTInfo << "It is also possible to define a default step size for all active volumes," << RESTendl;
        RESTInfo << "so that in case no step is defined, the default will be used." << RESTendl;
        cout << " " << endl;
        RESTInfo << "<storage sensitiveVolume=\"" << this->GetSensitiveVolume() << "\" maxStepSize=\""
                 << fMaxTargetStepSize << "mm\" />" << RESTendl;
        cout << " " << endl;
        GetChar();
    }

    // should return success or fail
}

///////////////////////////////////////////////
/// \brief Reads the biasing section defined inside TRestGeant4Metadata.
///
/// This section allows to define the size and properties of any number of
/// biasing volumes. Biasing volume properties include the multiplicity factor
/// and the range of energies that particles will be propagated to the next
/// biasing volume.
///
/// \code
///    <biasing value="off" type="virtualBox">
///      <biasingVolume size="2850mm" position="(0,0,0)mm" factor="2" energyRange="(0,5)MeV" />
///      <biasingVolume size="2450mm" position="(0,0,0)mm" factor="2" energyRange="(0,5)MeV" />
///      <biasingVolume size="2050mm" position="(0,0,0)mm" factor="2" energyRange="(0,5)MeV" />
///    </biasing>
/// \endcode
///
/// Check for more details in the general description of this class.
///
void TRestGeant4Metadata::ReadBiasing() {
    TiXmlElement* biasingDefinition = GetElement("biasing");
    if (biasingDefinition == nullptr) {
        fNBiasingVolumes = 0;
        return;
    }
    TString biasEnabled = GetFieldValue("value", biasingDefinition);
    TString biasType = GetFieldValue("type", biasingDefinition);

    RESTDebug << "Bias : " << biasEnabled << RESTendl;

    if (biasEnabled == "on" || biasEnabled == "ON" || biasEnabled == "On" || biasEnabled == "oN") {
        RESTInfo << "Biasing is enabled" << RESTendl;

        TiXmlElement* biasVolumeDefinition = GetElement("biasingVolume", biasingDefinition);
        Int_t n = 0;
        while (biasVolumeDefinition) {
            TRestGeant4BiasingVolume biasVolume;
            RESTDebug << "Def : " << biasVolumeDefinition << RESTendl;

            biasVolume.SetBiasingVolumePosition(
                Get3DVectorParameterWithUnits("position", biasVolumeDefinition));
            biasVolume.SetBiasingFactor(StringToDouble(GetParameter("factor", biasVolumeDefinition)));
            biasVolume.SetBiasingVolumeSize(GetDblParameterWithUnits("size", biasVolumeDefinition));
            biasVolume.SetEnergyRange(Get2DVectorParameterWithUnits("energyRange", biasVolumeDefinition));
            biasVolume.SetBiasingVolumeType(biasType);  // For the moment all the volumes should be same type

            /* TODO check that values are right if not printBiasingVolume with
            getchar() biasVolume.PrintBiasingVolume(); getchar();
            */

            fBiasingVolumes.push_back(biasVolume);
            biasVolumeDefinition = GetNextElement(biasVolumeDefinition);
            n++;
        }
        fNBiasingVolumes = n;
    }
}

///////////////////////////////////////////////
/// \brief Reads the generator section defined inside TRestGeant4Metadata.
///
/// This section allows to define the primary particles to be simulated.
/// The generator will allow us to define from where we will launch the
/// primary particles, their energy distribution, and their angular
/// momentum.
///
/// \code
///    <generator type="volume" from="gas" >
///      ///1. generator file
///      <source use="Xe136bb0n.dat"/>
///
///      ///2. decay0 package
///      <!--<source use="decay0" particle="Xe136" decayMode="0vbb" daughterLevel="3" />-->
///
///      ///3. geant4 internal
///      <!--<source use="geant4" particle="Na22" excitedLevel="0.0" fullChain="on">
///        <angularDist type="isotropic" />
///        <energyDist type="mono" energy="0.0" units="MeV" />
///      </source>-->
///    </generator>
/// \endcode
///
void TRestGeant4Metadata::ReadGenerator() {
    // TODO if some fields are defined in the generator but not finally used
    // i.e. <generator type="volume" from="gasTarget" position="(100,0,-100)
    // here position is irrelevant since the event will be generated from the
    // volume defined in the geometry we should take care of these values so they
    // are not stored in metadata (to avoid future confusion) In the particular
    // case of position, the value is overwritten in DetectorConstruction by the
    // center of the volume (i.e. gasTarget) but if i.e rotation or side are
    // defined and not relevant we should set it to -1

    TiXmlElement* generatorDefinition = GetElement("generator");

    fGeant4PrimaryGeneratorInfo.fSpatialGeneratorType = GetParameter("type", generatorDefinition, "volume");
    fGeant4PrimaryGeneratorInfo.fSpatialGeneratorShape = GetParameter("shape", generatorDefinition, "box");
    fGeant4PrimaryGeneratorInfo.fSpatialGeneratorFrom = GetParameter("from", generatorDefinition);
    if (fGeant4PrimaryGeneratorInfo.fSpatialGeneratorFrom != PARAMETER_NOT_FOUND_STR) {
        fGeant4PrimaryGeneratorInfo.fSpatialGeneratorShape = "gdml";
    }
    fGeant4PrimaryGeneratorInfo.fSpatialGeneratorSize =
        Get3DVectorParameterWithUnits("size", generatorDefinition);
    fGeant4PrimaryGeneratorInfo.fSpatialGeneratorPosition =
        Get3DVectorParameterWithUnits("position", generatorDefinition);
    fGeant4PrimaryGeneratorInfo.fSpatialGeneratorRotationAxis =
        StringTo3DVector(GetParameter("rotationAxis", generatorDefinition, "(0,0,1)"));
    fGeant4PrimaryGeneratorInfo.fSpatialGeneratorRotationValue =
        GetDblParameterWithUnits("rotationAngle", generatorDefinition);
    fGeant4PrimaryGeneratorInfo.fSpatialGeneratorSpatialDensityFunction =
        GetParameter("densityFunc", generatorDefinition, "1");

    TiXmlElement* sourceDefinition = GetElement("source", generatorDefinition);
    while (sourceDefinition) {
        string use = GetParameter("use", sourceDefinition, "");

        TRestGeant4ParticleSource* source = TRestGeant4ParticleSource::instantiate(use);
        ReadParticleSource(source, sourceDefinition);
        AddParticleSource(source);

        sourceDefinition = GetNextElement(sourceDefinition);
    }

    // check if the generator is valid.
    if (GetNumberOfSources() == 0) {
        RESTError << "No sources are added inside generator!" << RESTendl;
        exit(1);
    }
}

///////////////////////////////////////////////
/// \brief It reads the <source definition given by argument
///
void TRestGeant4Metadata::ReadParticleSource(TRestGeant4ParticleSource* source, TiXmlElement* definition) {
    TiXmlElement* sourceDefinition = definition;

    source->SetParticleName(GetParameter("particle", sourceDefinition));

    source->SetExcitationLevel(StringToDouble(GetParameter("excitedLevel", sourceDefinition)));

    Int_t charge = 0;
    if (GetParameter("charge", sourceDefinition) == PARAMETER_NOT_FOUND_STR)
        charge = 0;
    else
        charge = StringToInteger(GetParameter("charge", sourceDefinition));

    source->SetParticleCharge(charge);

    TString fullChain = GetParameter("fullChain", sourceDefinition);

    if (fullChain == "on" || fullChain == "ON" || fullChain == "On" || fullChain == "oN") {
        SetFullChain(true);
    } else {
        SetFullChain(false);
    }

    // Angular distribution parameters
    TiXmlElement* angularDefinition = GetElement("angularDist", sourceDefinition);
    source->SetAngularDistType(GetParameter("type", angularDefinition, "flux"));
    if (source->GetAngularDistType() == "TH1D") {
        source->SetAngularFilename(SearchFile(GetParameter("file", angularDefinition)));
        source->SetAngularName(GetParameter("spctName", angularDefinition));
    }
    if (GetNumberOfSources() == 0 && source->GetAngularDistType() == "backtoback") {
        cout << "WARNING: First source cannot be backtoback. Setting it to isotropic" << endl;
        source->SetAngularDistType("isotropic");
    }
    source->SetDirection(StringTo3DVector(GetParameter("direction", angularDefinition, "(1,0,0)")));

    // Energy distribution parameters
    TiXmlElement* energyDefinition = GetElement("energyDist", sourceDefinition);
    source->SetEnergyDistType(GetParameter("type", energyDefinition, "mono"));
    if (source->GetEnergyDistType() == "TH1D") {
        source->SetSpectrumFilename(SearchFile(GetParameter("file", energyDefinition)));
        source->SetSpectrumName(GetParameter("spctName", energyDefinition));
    }
    source->SetEnergyRange(Get2DVectorParameterWithUnits("range", energyDefinition));
    if (source->GetEnergyDistType() == "mono") {
        Double_t en;
        en = GetDblParameterWithUnits("energy", energyDefinition, 0);
        source->SetEnergyRange(TVector2(en, en));
        source->SetEnergy(en);
    }

    // allow custom configuration from the class
    source->LoadConfigFromElement(sourceDefinition, fElementGlobal, fVariables);
    // AddParticleSource(source);
}

void TRestGeant4Metadata::RemoveParticleSources() {
    for (auto c : fParticleSource) {
        delete c;
    }
    fParticleSource.clear();
}

void TRestGeant4Metadata::AddParticleSource(TRestGeant4ParticleSource* src) {
    fParticleSource.push_back(src);
}

///////////////////////////////////////////////
/// \brief Reads the storage section defined inside TRestGeant4Metadata.
///
/// This section allows to define which hits will be stored to disk.
/// Different volumes in the geometry can be tagged for hit storage.
///
/// \code
///    <storage sensitiveVolume="gas">
///      <parameter name="energyRange" value="(0,5)MeV" />
///      <activeVolume name="gas" chance="1" maxStepSize="2mm" />
///    </storage>
/// \endcode
///
void TRestGeant4Metadata::ReadStorage() {
    TiXmlElement* storageDefinition = GetElement("storage");
    fSensitiveVolume = GetFieldValue("sensitiveVolume", storageDefinition);
    if (fSensitiveVolume == "Not defined") {
        RESTWarning << "Sensitive volume not defined. Setting it to 'gas'!" << RESTendl;
        fSensitiveVolume = "gas";
    }
    Double_t defaultStep = GetDblParameterWithUnits("maxStepSize", storageDefinition);
    if (defaultStep < 0) defaultStep = 0;

    RESTInfo << "Sensitive volume: " << fSensitiveVolume << RESTendl;

    fEnergyRangeStored = Get2DVectorParameterWithUnits("energyRange", storageDefinition);

    auto gdml = new TRestGDMLParser();
    gdml->Load(GetGdmlFilename().Data());

    fGeant4GeometryInfo.PopulateFromGdml(gdml->GetOutputGDMLFile());

    const auto& physicalVolumes = fGeant4GeometryInfo.fGdmlNewPhysicalNames;
    TiXmlElement* volumeDefinition = GetElement("activeVolume", storageDefinition);
    while (volumeDefinition) {
        Double_t chance = StringToDouble(GetFieldValue("chance", volumeDefinition));
        if (chance == -1) chance = 1;

        Double_t maxStep = GetDblParameterWithUnits("maxStepSize", volumeDefinition);
        if (maxStep < 0) maxStep = defaultStep;

        const TString& name = GetFieldValue("name", volumeDefinition);
        // first we verify it's in the list of valid volumes
        if (!fGeant4GeometryInfo.IsValidGdmlName(name)) {
            bool isValidLogical = false;
            for (size_t i = 0; i < fGeant4GeometryInfo.fGdmlNewPhysicalNames.size(); i++) {
                if (fGeant4GeometryInfo.fGdmlLogicalNames[i] == name) {
                    isValidLogical = true;
                    const auto& gdmlName = fGeant4GeometryInfo.fGdmlNewPhysicalNames[i];
                    RESTInfo << "Adding active volume from RML: '" << gdmlName << "' from logical volume: '"
                             << name << "' with chance: " << chance << RESTendl;
                    SetActiveVolume(gdmlName, chance, maxStep);
                }
            }
            if (!isValidLogical) {
                RESTError << "TRestGeant4Metadata: Problem reading storage section." << RESTendl;
                RESTError << " 	- The volume '" << name << "' was not found in the GDML geometry."
                          << RESTendl;
                exit(1);
            }
        } else {
            RESTInfo << "Adding active volume from RML: '" << name << "' with chance: " << chance << RESTendl;
            SetActiveVolume(name, chance, maxStep);
        }
        volumeDefinition = GetNextElement(volumeDefinition);
    }

    // If the user did not add explicitly any volume to the storage section we understand
    // the user wants to register all the volumes
    if (GetNumberOfActiveVolumes() == 0) {
        for (auto& name : physicalVolumes) {
            SetActiveVolume(name, 1, defaultStep);
            RESTInfo << "Automatically adding active volume: '" << name << "' with chance: " << 1 << RESTendl;
        }
    }
}

///////////////////////////////////////////////
/// \brief Prints on screen the details about the Geant4 simulation
/// conditions, stored in TRestGeant4Metadata.
///
void TRestGeant4Metadata::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "Geant 4 version : " << GetGeant4Version() << RESTendl;
    RESTMetadata << "Random seed : " << GetSeed() << RESTendl;
    RESTMetadata << "GDML geometry : " << GetGdmlReference() << RESTendl;
    RESTMetadata << "GDML materials reference : " << GetMaterialsReference() << RESTendl;
    RESTMetadata << "Sub-event time delay : " << GetSubEventTimeDelay() << " us" << RESTendl;
    Double_t mx = GetMagneticField().X();
    Double_t my = GetMagneticField().Y();
    Double_t mz = GetMagneticField().Z();
    RESTMetadata << "Magnetic field : ( " << mx << ", " << my << ", " << mz << ") T" << RESTendl;
    if (fSaveAllEvents) RESTMetadata << "Save all events was enabled!" << RESTendl;
    if (fRegisterEmptyTracks)
        RESTMetadata << "Register empty tracks was enabled" << RESTendl;
    else
        RESTMetadata << "Register empty tracks was NOT enabled" << RESTendl;
    RESTMetadata << "   ++++++++++ Generator +++++++++++   " << RESTendl;
    RESTMetadata << "Number of generated events : " << GetNumberOfEvents() << RESTendl;
    fGeant4PrimaryGeneratorInfo.Print();

    for (int i = 0; i < GetNumberOfSources(); i++) GetParticleSource(i)->PrintParticleSource();

    RESTMetadata << "   ++++++++++ Storage volumes +++++++++++   " << RESTendl;
    RESTMetadata << "Energy range : Emin = " << GetMinimumEnergyStored()
                 << ", Emax : " << GetMaximumEnergyStored() << RESTendl;
    RESTMetadata << "Sensitive volume : " << GetSensitiveVolume() << RESTendl;
    RESTMetadata << "Active volumes : " << GetNumberOfActiveVolumes() << RESTendl;
    RESTMetadata << "---------------------------------------" << RESTendl;
    for (int n = 0; n < GetNumberOfActiveVolumes(); n++) {
        RESTMetadata << "Name : " << GetActiveVolumeName(n)
                     << ", ID : " << GetActiveVolumeID(GetActiveVolumeName(n))
                     << ", maxStep : " << GetMaxStepSize(GetActiveVolumeName(n)) << "mm "
                     << ", chance : " << GetStorageChance(GetActiveVolumeName(n)) << RESTendl;
    }
    for (int n = 0; n < GetNumberOfBiasingVolumes(); n++) {
        GetBiasingVolume(n).PrintBiasingVolume();
    }
    RESTMetadata << "+++++" << RESTendl;
}

/////////////////////////////////////////////////
///// \brief Reads an input file produced by Decay0.
/////
///// The input file should contain the description of several
///// pre-generated events, providing the names (or ids) of
///// particles to be produced, their energy, and momentum.
///// The particles and their properties are stored in a
///// TRestG4ParticleCollection which will be randomly accessed
///// by the restG4 package.
/////
///// \param fName The Decay0 filename located at
///// REST_PATH/data/generator/
/////
// void TRestGeant4Metadata::ReadEventDataFile(TString fName) {
//    string fullPathName = SearchFile((string)fName);
//    if (fullPathName == "") {
//        ferr << "File not found : " << fName << endl;
//        ferr << "Decay0 generator file could not be found!!" << endl;
//        exit(1);
//    }
//
//     debug << "TRestGeant4Metadata::ReadGeneratorFile" << endl;
//     debug << "Full path generator file : " << fullPathName << endl;
//
//     if (!ReadOldDecay0File(fullPathName)) ReadNewDecay0File(fullPathName);
// }
//
/////////////////////////////////////////////////
///// \brief Reads particle information using the file format from newer Decay0 versions.
/////
///// This is an auxiliar method used in TRestGeant4Metadata::ReadEventDataFile that will read the Decay0
/// files
///// produced with the newer Decay0 versions.
/////
// Int_t TRestGeant4Metadata::ReadNewDecay0File(TString fileName) {
//    ifstream infile;
//    infile.open(fileName);
//    if (!infile.is_open()) {
//        printf("Error when opening file %s\n", fileName.Data());
//        return 0;
//    }
//
//     int generatorEvents = 0;
//     string s;
//     // First lines are discarded.
//     for (int i = 0; i < 24; i++) {
//         getline(infile, s);
//         int pos = s.find("@nevents=");
//         if (pos != -1) generatorEvents = StringToInteger(s.substr(10, s.length() - 10));
//     }
//
//     if (generatorEvents == 0) {
//         ferr << "TRestGeant4Metadata::ReadNewDecay0File. Problem reading generator file" << endl;
//         exit(1);
//     }
//
//     TRestGeant4ParticleSource* src = TRestGeant4ParticleSource::instantiate();
//     src->SetGenFilename(fileName);
//     // src->SetAngularDistType("flux");
//     // src->SetEnergyDistType("mono");
//
//     TRestGeant4Particle particle;
//
//     debug << "Reading generator file : " << fileName << endl;
//     debug << "Total number of events : " << generatorEvents << endl;
//     for (int n = 0; n < generatorEvents && !infile.eof(); n++) {
//         int pos = -1;
//         while (!infile.eof() && pos == -1) {
//             getline(infile, s);
//             pos = s.find("@event_start");
//         }
//
//         // Time - nuclide is skipped
//         getline(infile, s);
//
//         Int_t nParticles;
//         infile >> nParticles;
//         debug << "Number of particles : " << nParticles << endl;
//
//         // cout << evID <<" "<< time <<" "<< nParticles <<" "<< endl;
//         for (int i = 0; i < nParticles && !infile.eof(); i++) {
//             Int_t pID;
//             Double_t momx, momy, momz, mass;
//             Double_t energy = -1, momentum2;
//             TString pName;
//
//             Double_t time;
//             infile >> pID >> time >> momx >> momy >> momz >> pName;
//
//             debug << " ---- New particle found --- " << endl;
//             debug << " Particle name : " << pName << endl;
//             debug << " - pId : " << pID << endl;
//             debug << " - Relative time : " << time << endl;
//             debug << " - px: " << momx << " py: " << momy << " pz: " << momz << " " << endl;
//
//             if (pID == 3) {
//                 momentum2 = (momx * momx) + (momy * momy) + (momz * momz);
//                 mass = 0.511;
//
//                 energy = TMath::Sqrt(momentum2 + mass * mass) - mass;
//                 particle.SetParticleName("e-");
//                 particle.SetParticleCharge(-1);
//                 particle.SetExcitationLevel(0);
//
//             } else if (pID == 1) {
//                 momentum2 = (momx * momx) + (momy * momy) + (momz * momz);
//
//                 energy = TMath::Sqrt(momentum2);
//                 particle.SetParticleName("gamma");
//                 particle.SetParticleCharge(0);
//                 particle.SetExcitationLevel(0);
//             } else {
//                 cout << "Particle id " << pID << " not recognized" << endl;
//             }
//
//             TVector3 momDirection(momx, momy, momz);
//             momDirection = momDirection.Unit();
//
//             particle.SetEnergy(1000. * energy);
//             particle.SetDirection(momDirection);
//
//             src->AddParticle(particle);
//         }
//         src->FlushParticlesTemplate();
//     }
//
//     AddParticleSource(src);
//
//     return 1;
// }
//
/////////////////////////////////////////////////
///// \brief Reads particle information using the file format from older Decay0 versions.
/////
///// This is an auxiliar method used in TRestGeant4Metadata::ReadEventDataFile that will read the Decay0
/// files
///// produced with the newer Decay0 versions.
/////
// Int_t TRestGeant4Metadata::ReadOldDecay0File(TString fileName) {
//    ifstream infile;
//    infile.open(fileName);
//    if (!infile.is_open()) {
//        printf("Error when opening file %s\n", fileName.Data());
//        return 0;
//    }
//
//    string s;
//    // First lines are discarded.
//    int headerFound = 0;
//    for (int i = 0; i < 30; i++) {
//        getline(infile, s);
//        if (s.find("#!bxdecay0 1.0.0") != -1) return 0;
//        if (s.find("First event and full number of events:") != -1) {
//            headerFound = 1;
//            break;
//        }
//    }
//    if (!headerFound) {
//        ferr
//            << "TRestGeant4Metadata::ReadOldDecay0File. Problem reading generator file: no \"First event and
//            "
//               "full number of events:\" header.\n";
//        abort();
//    }
//    int tmpInt;
//    int fGeneratorEvents;
//    infile >> tmpInt >> fGeneratorEvents;
//
//     // cout << "i : " << tmpInt << " fN : " << fGeneratorEvents << endl;
//     TRestGeant4ParticleSource* src = TRestGeant4ParticleSource::instantiate();
//     src->SetGenFilename(fileName);
//     // src->SetAngularDistType("flux");
//     // src->SetEnergyDistType("mono");
//
//     TRestGeant4Particle particle;
//     string type = (string)GetGeneratorType();
//
//     cout << "Reading generator file : " << fileName << endl;
//     cout << "Total number of events : " << fGeneratorEvents << endl;
//     for (int n = 0; n < fGeneratorEvents && !infile.eof(); n++) {
//         Int_t nParticles;
//         Int_t evID;
//         Double_t time;
//
//         infile >> evID >> time >> nParticles;
//
//         // cout << evID <<" "<< time <<" "<< nParticles <<" "<< endl;
//         for (int i = 0; i < nParticles && !infile.eof(); i++) {
//             Int_t pID;
//             Double_t momx, momy, momz, mass;
//             Double_t energy = -1, momentum2;
//             Double_t x, y, z;
//
//             infile >> pID >> momx >> momy >> momz >> time;
//             if (type == "file") infile >> x >> y >> z;
//
//             // cout << momx << " " << momy << " " << momz << " " << endl;
//
//             bool ise = 2 <= pID && pID <= 3, ismu = 5 <= pID && pID <= 6, isp = pID == 14, isg = pID == 1;
//             if (ise || ismu || isp || isg) {
//                 momentum2 = (momx * momx) + (momy * momy) + (momz * momz);
//                 if (ise) {
//                     mass = 0.511;
//                     particle.SetParticleName(pID == 2 ? "e+" : "e-");
//                     particle.SetParticleCharge(pID == 2 ? 1 : -1);
//                 } else if (ismu) {
//                     mass = 105.7;
//                     particle.SetParticleName(pID == 5 ? "mu+" : "mu-");
//                     particle.SetParticleCharge(pID == 5 ? 1 : -1);
//                 } else if (isp) {
//                     mass = 938.3;
//                     particle.SetParticleName("proton");
//                     particle.SetParticleCharge(1);
//                 } else {
//                     mass = 0;
//                     particle.SetParticleName("gamma");
//                     particle.SetParticleCharge(0);
//                 }
//
//                 energy = TMath::Sqrt(momentum2 + mass * mass) - mass;
//                 particle.SetExcitationLevel(0);
//             } else {
//                 cout << "Particle id " << pID << " not recognized" << endl;
//             }
//
//             TVector3 momDirection(momx, momy, momz);
//             momDirection = momDirection.Unit();
//
//             particle.SetEnergy(1000. * energy);
//             particle.SetDirection(momDirection);
//             particle.SetOrigin(TVector3(x, y, z));
//
//             src->AddParticle(particle);
//         }
//
//         src->FlushParticlesTemplate();
//     }
//
//     AddParticleSource(src);
//
//     return 1;
// }
//
/////////////////////////////////////////////////
///// \brief It reads the <source definition given by argument
/////
// void TRestGeant4Metadata::ReadParticleSource(TiXmlElement* definition) {
//    TiXmlElement* sourceDefinition = definition;
//
//     TRestGeant4ParticleSource* source = new TRestGeant4ParticleSource();
//
//     source->SetParticleName(GetFieldValue("particle", sourceDefinition));
//
//     source->SetExcitationLevel(StringToDouble(GetFieldValue("excitedLevel", sourceDefinition)));
//
//     Int_t charge = 0;
//     if (GetFieldValue("charge", sourceDefinition) == "Not defined")
//         charge = 0;
//     else
//         charge = StringToInteger(GetFieldValue("charge", sourceDefinition));
//
//     source->SetParticleCharge(charge);
//
//     TString fullChain = GetFieldValue("fullChain", sourceDefinition);
//
//     if (fullChain == "on" || fullChain == "ON" || fullChain == "On" || fullChain == "oN") {
//         SetFullChain(true);
//     } else {
//         SetFullChain(false);
//     }
//
//     // Angular distribution parameters
//     TiXmlElement* angularDefinition = GetElement("angularDist", sourceDefinition);
//
//     source->SetAngularDistType(GetFieldValue("type", angularDefinition));
//
//     if (source->GetAngularDistType() == "TH1D") {
//         source->SetAngularFilename(SearchFile(GetFieldValue("file", angularDefinition)));
//         source->SetAngularName(GetFieldValue("spctName", angularDefinition));
//     }
//
//     if (GetNumberOfSources() == 0 && source->GetAngularDistType() == "backtoback") {
//         cout << "WARNING: First source cannot be backtoback. Setting it to isotropic" << endl;
//         source->SetAngularDistType("isotropic");
//     }
//
//     source->SetDirection(StringTo3DVector(GetFieldValue("direction", angularDefinition)));
//
//     // Energy distribution parameters
//     TiXmlElement* energyDefinition = GetElement("energyDist", sourceDefinition);
//
//     source->SetEnergyDistType(GetFieldValue("type", energyDefinition));
//
//     if (source->GetEnergyDistType() == "TH1D") {
//         source->SetSpectrumFilename(SearchFile(GetFieldValue("file", energyDefinition)));
//         source->SetSpectrumName(GetFieldValue("spctName", energyDefinition));
//     }
//
//     source->SetEnergyRange(Get2DVectorParameterWithUnits("range", energyDefinition));
//
//     if (source->GetEnergyDistType() == "mono") {
//         Double_t en;
//         en = GetDblParameterWithUnits("energy", energyDefinition);
//         source->SetEnergyRange(TVector2(en, en));
//         source->SetEnergy(en);
//     }
//
//     AddParticleSource(source);
// }

///////////////////////////////////////////////
/// \brief Returns the id of an active volume giving as parameter its name.
Int_t TRestGeant4Metadata::GetActiveVolumeID(TString name) {
    Int_t id;
    for (id = 0; id < (Int_t)fActiveVolumes.size(); id++) {
        if (fActiveVolumes[id] == name) return id;
    }
    return -1;
}

///////////////////////////////////////////////
/// \brief Adds a geometry volume to the list of active volumes.
///
/// \param name The name of the volume to be added to the active volumes list
/// for storage. Using GDML naming convention.
///
/// \param chance Probability that for a particular event the hits are stored in
/// that volume.
///
/// \param maxStep It defines the maximum integration step at the active volume.
///
/// The aim of this parameter is to define control volumes. Usually the volume
/// of interest will be always registered (chance=1).
///
void TRestGeant4Metadata::SetActiveVolume(const TString& name, Double_t chance, Double_t maxStep) {
    for (size_t i = 0; i < fActiveVolumes.size(); i++) {
        const auto activeVolumeName = fActiveVolumes[i];
        if (name == activeVolumeName) {
            fChance[i] = chance;
            fMaxStepSize[i] = maxStep;
            return;
        }
    }
    fActiveVolumes.push_back(name);
    fChance.push_back(chance);
    fMaxStepSize.push_back(maxStep);
}

///////////////////////////////////////////////
/// \brief Returns true if the volume named *volName* has been registered for
/// data storage.
///
Bool_t TRestGeant4Metadata::isVolumeStored(TString volName) {
    for (int n = 0; n < GetNumberOfActiveVolumes(); n++)
        if (GetActiveVolumeName(n) == volName) return true;

    return false;
}

///////////////////////////////////////////////
/// \brief Returns the probability of an active volume being stored
///
Double_t TRestGeant4Metadata::GetStorageChance(TString vol) {
    Int_t id;
    for (id = 0; id < (Int_t)fActiveVolumes.size(); id++) {
        if (fActiveVolumes[id] == vol) return fChance[id];
    }
    RESTWarning << "TRestGeant4Metadata::GetStorageChance. Volume " << vol << " not found" << RESTendl;

    return 0;
}

///////////////////////////////////////////////
/// \brief Returns the maximum step at a particular active volume
///
Double_t TRestGeant4Metadata::GetMaxStepSize(TString vol) {
    for (Int_t id = 0; id < (Int_t)fActiveVolumes.size(); id++) {
        if (fActiveVolumes[id] == vol) return fMaxStepSize[id];
    }
    RESTWarning << "TRestGeant4Metadata::GetMaxStepSize. Volume " << vol << " not found" << RESTendl;

    return 0;
}

size_t TRestGeant4Metadata::GetGeant4VersionMajor() const {
    TString majorVersion = "";
    for (int i = 0; i < fGeant4Version.Length(); i++) {
        auto c = fGeant4Version[i];
        if (c == '.') {
            break;
        }
        majorVersion += c;
    }
    return std::stoi(majorVersion.Data());
}
