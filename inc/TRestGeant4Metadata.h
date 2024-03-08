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

#ifndef RestCore_TRestGeant4Metadata
#define RestCore_TRestGeant4Metadata

#include <TMath.h>
#include <TRestMetadata.h>
#include <TString.h>
#include <TVector2.h>
#include <TVector3.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

#include "TRestGeant4BiasingVolume.h"
#include "TRestGeant4GeometryInfo.h"
#include "TRestGeant4ParticleSource.h"
#include "TRestGeant4PhysicsInfo.h"
#include "TRestGeant4PrimaryGeneratorInfo.h"

/// The main class to store the *Geant4* simulation conditions that will be used by *restG4*.
class TRestGeant4Metadata : public TRestMetadata {
   private:
    void Initialize() override;

    void InitFromConfigFile() override;

    void ReadGenerator();
    void ReadParticleSource(TRestGeant4ParticleSource* src, TiXmlElement* sourceDefinition);

    void ReadDetector();
    void ReadBiasing();

    // Metadata is the result of a merge of other metadata
    bool fIsMerge = false;

    /// Class used to store and retrieve geometry info
    TRestGeant4GeometryInfo fGeant4GeometryInfo;

    /// Class used to store and retrieve physics info such as process names or particle names
    TRestGeant4PhysicsInfo fGeant4PhysicsInfo;

    /// Class used to store and retrieve Geant4 primary generator info
    TRestGeant4PrimaryGeneratorInfo fGeant4PrimaryGeneratorInfo;

    /// The version of Geant4 used to generate the data
    TString fGeant4Version;

    /// The local path to the GDML geometry
    TString fGeometryPath;  //!

    /// The filename of the GDML geometry
    TString fGdmlFilename;  //!

    /// A GDML geometry reference introduced in the header of the GDML main setup
    TString fGdmlReference;

    /// A GDML materials reference introduced in the header of the GDML of materials definition
    TString fMaterialsReference;

    /// \brief A 2d-std::vector storing the energy range, in keV, to decide if a particular event should be
    /// written to disk or not.
    TVector2 fEnergyRangeStored = {0, 1E20};

    /// \brief A std::vector to store the names of the active volumes.
    std::vector<TString> fActiveVolumes;

    /// \brief A std::vector to store the probability value to write to disk the hits in a particular event.
    std::vector<Double_t> fChance;

    /// \brief A std::vector to store the maximum step size at a particular volume.
    std::vector<Double_t> fMaxStepSize;

    /// \brief It the defines the primary source properties, particle type, momentum, energy, etc.
    std::vector<TRestGeant4ParticleSource*> fParticleSource;  //->

    /// \brief The number of biasing volumes used in the simulation. If zero, no biasing technique is used.
    Int_t fNBiasingVolumes = 0;

    /// A std::vector containing the biasing volume properties.
    std::vector<TRestGeant4BiasingVolume> fBiasingVolumes;

    /// \brief The maximum target step size, in mm, allowed in Geant4 for the target
    /// volume (usually the gas volume). It is obsolete now. We define it at the active volume.
    Double_t fMaxTargetStepSize = 0;  //!

    /// \brief A time gap, in us, determining if an energy hit should be considered (and
    /// stored) as an independent event.
    Double_t fSubEventTimeDelay = 100;

    /// \brief Defines if a radioactive isotope decay is simulated in full chain
    /// (fFullChain=true), or just a single decay (fFullChain=false).
    Bool_t fFullChain = true;

    /// \brief The volume that serves as trigger for data storage. Only events that
    /// deposit a non-zero energy on this volume will be registered.
    std::vector<TString> fSensitiveVolumes;

    /// \brief The number of events simulated, or to be simulated.
    Long64_t fNEvents = 0;

    /// \brief The number of events the user requested to be on the file
    Long64_t fNRequestedEntries = 0;

    /// \brief Time before simulation is ended and saved
    Int_t fSimulationMaxTimeSeconds = 0;

    /// \brief The time, in seconds, that the simulation took to complete.
    Long64_t fSimulationTime = 0;

    /// \brief The seed value used for Geant4 random event generator.
    /// If it is zero its value will be assigned using the system timestamp.
    Long_t fSeed = 0;

    /// \brief If the simulation is produced as a merge of multiple files, this stored the seeds of the
    /// individual files
    std::vector<Long_t> fMergeSeeds;

    /// \brief If the simulation is produced as a merge of multiple files, this stored the number primaries
    /// for the individual files
    std::vector<Long_t> fMergeNEvents;

    /// \brief If this parameter is set to 'true' it will save all events even if they leave no energy in the
    /// sensitive volume (used for debugging purposes). It is set to 'false' by default.
    Bool_t fSaveAllEvents = false;

    /// \brief Option to store target isotope information on hadronic processes (disabled by default)
    Bool_t fStoreHadronicTargetInfo = false;

    /// \brief Sets all volume as active without having to explicitly list them.
    Bool_t fActivateAllVolumes = false;  //!

    /// \brief If activated will remove tracks not present in volumes marked as "keep" or "sensitive".
    Bool_t fRemoveUnwantedTracks = false;

    /// \brief Option for 'removeUnwantedTracks', if enabled tracks with hits in volumes will be kept event if
    /// they deposit zero energy (such as neutron captures)
    Bool_t fRemoveUnwantedTracksKeepZeroEnergyTracks = false;

    /// \brief A container related to fRemoveUnwantedTracks.
    std::set<std::string> fRemoveUnwantedTracksVolumesToKeep;

    std::set<std::string> fKillVolumes;

    /// \brief If this parameter is set to 'true' it will print out on screen every time 10k events are
    /// reached.
    Bool_t fPrintProgress = false;  //!

    /// \brief If this parameter is enabled it will register tracks with no hits inside. This is the default
    /// behaviour. If it is disabled then empty tracks will not be written to disk at the risk of loosing
    /// traceability, but saving disk space and likely improving computing performance for large events.
    Bool_t fRegisterEmptyTracks = true;

    /// \brief The world magnetic field
    TVector3 fMagneticField = TVector3(0, 0, 0);

   public:
    std::set<std::string> fActiveVolumesSet = {};  //! // Used for faster lookup

    /// \brief Returns the random seed that was used to generate the corresponding geant4 dataset.
    inline Long_t GetSeed() const { return fSeed; }

    /// \brief Returns an immutable reference to the geometry info
    inline const TRestGeant4GeometryInfo& GetGeant4GeometryInfo() const { return fGeant4GeometryInfo; }

    /// \brief Returns an immutable reference to the physics info
    inline const TRestGeant4PhysicsInfo& GetGeant4PhysicsInfo() const { return fGeant4PhysicsInfo; }

    /// \brief Returns an immutable reference to the primary generator info
    inline const TRestGeant4PrimaryGeneratorInfo& GetGeant4PrimaryGeneratorInfo() const {
        return fGeant4PrimaryGeneratorInfo;
    }

    /// \brief Returns a std::string with the version of Geant4 used on the event data simulation
    inline TString GetGeant4Version() const { return fGeant4Version; }

    size_t GetGeant4VersionMajor() const;

    bool GetStoreHadronicTargetInfo() const { return fStoreHadronicTargetInfo; }

    /// Returns the local path to the GDML geometry
    inline TString GetGeometryPath() const { return fGeometryPath; }

    /// Returns the main filename of the GDML geometry
    inline TString GetGdmlFilename() const { return fGdmlFilename; }

    /// Returns the reference provided at the GDML file header
    inline TString GetGdmlReference() const { return fGdmlReference; }

    /// Returns the reference provided at the materials file header
    inline TString GetMaterialsReference() const { return fMaterialsReference; }

    /// \brief Returns true in case full decay chain simulation is enabled.
    inline Bool_t isFullChainActivated() const { return fFullChain; }

    /// \brief Returns the value of the maximum Geant4 step size in mm for the
    /// target volume.
    inline Double_t GetMaxTargetStepSize() const { return fMaxTargetStepSize; }

    /// \brief Returns the time gap, in us, required to consider a Geant4 hit as a
    /// new independent event. It is used to separate simulated events that in
    /// practice will appear as such in our detector. I.e. to separate multiple
    /// decay products (sometimes with years time delays) into independent events.
    inline Double_t GetSubEventTimeDelay() const { return fSubEventTimeDelay; }

    /// It returns true if save all events is active
    inline Bool_t GetSaveAllEvents() const { return fSaveAllEvents; }

    /// It returns true if `printProgress` parameter was set to true
    inline Bool_t PrintProgress() const { return fPrintProgress; }

    /// It returns false if `registerEmptyTracks` parameter was set to false.
    inline Bool_t RegisterEmptyTracks() const { return fRegisterEmptyTracks; }

    /// \brief Used exclusively by restG4 to set the value of the random seed used on Geant4 simulation.
    inline void SetSeed(Long_t seed) { fSeed = seed; }

    /// Enables or disables the save all events feature
    inline void SetSaveAllEvents(const Bool_t value) { fSaveAllEvents = value; }

    /// Sets the value of the Geant4 version
    inline void SetGeant4Version(const TString& versionString) { fGeant4Version = versionString; }

    ///  Enables/disables the full chain decay generation.
    inline void SetFullChain(Bool_t fullChain) { fFullChain = fullChain; }

    /// It sets the location of the geometry files
    inline void SetGeometryPath(std::string path) { fGeometryPath = path; }

    /// It sets the main filename to be used for the GDML geometry
    inline void SetGdmlFilename(std::string gdmlFile) { fGdmlFilename = gdmlFile; }

    /// Returns the reference provided at the GDML file header
    inline void SetGdmlReference(std::string reference) { fGdmlReference = reference; }

    /// Returns the reference provided at the materials file header
    inline void SetMaterialsReference(std::string reference) { fMaterialsReference = reference; }

    /// Returns the number of events to be simulated.
    inline Long64_t GetNumberOfEvents() const { return fNEvents; }

    inline Long64_t GetNumberOfRequestedEntries() const { return fNRequestedEntries; }

    inline Int_t GetSimulationMaxTimeSeconds() const { return fSimulationMaxTimeSeconds; }

    inline Long64_t GetSimulationTime() const { return fSimulationTime; }

    // Direct access to sources definition in primary generator
    ///////////////////////////////////////////////////////////
    /// Returns the number of primary event sources defined in the generator.
    inline Int_t GetNumberOfSources() const { return fParticleSource.size(); }

    /// Returns the name of the particle source with index n (Geant4 based names).
    inline TRestGeant4ParticleSource* GetParticleSource(size_t n = 0) const { return fParticleSource[n]; }

    /// Remove all the particle sources.
    void RemoveParticleSources();

    /// Adds a new particle source.
    void AddParticleSource(TRestGeant4ParticleSource* src);
    ///////////////////////////////////////////////////////////

    // Direct access to biasing volumes definition
    //////////////////////////////////////////////
    /// Returns the number of biasing volumes defined

    inline size_t GetNumberOfBiasingVolumes() const { return fBiasingVolumes.size(); }

    /// Return the biasing volume with index n
    inline TRestGeant4BiasingVolume GetBiasingVolume(int n) { return fBiasingVolumes[n]; }

    /// \brief Returns the number of biasing volumes defined. If 0 the biasing technique is not being used.
    inline Int_t isBiasingActive() const { return fBiasingVolumes.size(); }

    /// Returns a std::string with the name of the sensitive volume.
    inline TString GetSensitiveVolume(int n = 0) const { return fSensitiveVolumes[n]; }

    inline size_t GetNumberOfSensitiveVolumes() const { return fSensitiveVolumes.size(); }

    inline const std::vector<TString>& GetSensitiveVolumes() const { return fSensitiveVolumes; }

    /// Sets the name of the sensitive volume
    inline void SetNumberOfEvents(Long64_t n) { fNEvents = n; }

    inline void SetNumberOfRequestedEntries(Long64_t n) { fNRequestedEntries = n; }

    inline void SetSimulationMaxTimeSeconds(Int_t seconds) { fSimulationMaxTimeSeconds = seconds; }

    /// Sets the name of the sensitive volume
    inline void InsertSensitiveVolume(const TString& volume) {
        for (const auto& sensitiveVolume : fSensitiveVolumes) {
            // Do not add duplicate volumes
            if (volume == sensitiveVolume) {
                return;
            }
        }
        fSensitiveVolumes.push_back(volume);
    }

    /// \brief Returns the probability per event to register (write to disk) hits in the storage volume with
    /// index n.
    inline Double_t GetStorageChance(Int_t n) const { return fChance[n]; }

    /// Returns the probability per event to register (write to disk) hits in a GDML volume given its geometry
    /// name.
    Double_t GetStorageChance(const TString& volume);

    Double_t GetMaxStepSize(const TString& volume);

    /// Returns the minimum event energy required for an event to be stored.
    inline Double_t GetMinimumEnergyStored() const { return fEnergyRangeStored.X(); }

    /// Returns the maximum event energy required for an event to be stored.
    inline Double_t GetMaximumEnergyStored() const { return fEnergyRangeStored.Y(); }

    /// \brief Returns the number of active volumes, or geometry volumes that have been
    /// selected for data storage.
    inline unsigned int GetNumberOfActiveVolumes() const { return fActiveVolumes.size(); }

    inline bool IsActiveVolume(const char* volumeName) const {
        return fActiveVolumesSet.count(volumeName) > 0;
    }  //!

    inline bool IsKeepTracksVolume(const char* volumeName) const {
        return fRemoveUnwantedTracksVolumesToKeep.count(volumeName) > 0;
    }

    inline bool IsKillVolume(const char* volumeName) const { return fKillVolumes.count(volumeName) > 0; }

    inline std::vector<std::string> GetKillVolumes() const {
        std::vector<std::string> result;
        for (const auto& volume : fKillVolumes) {
            result.emplace_back(volume);
        }
        return result;
    }

    inline std::vector<std::string> GetRemoveUnwantedTracksVolumesToKeep() const {
        std::vector<std::string> result;
        for (const auto& volume : fRemoveUnwantedTracksVolumesToKeep) {
            result.emplace_back(volume);
        }
        return result;
    }

    Double_t GetCosmicFluxInCountsPerCm2PerSecond() const;
    Double_t GetCosmicIntensityInCountsPerSecond() const;
    Double_t GetEquivalentSimulatedTime() const;

    /// Returns a std::string with the name of the active volume with index n
    inline TString GetActiveVolumeName(Int_t n) const { return fActiveVolumes[n]; }

    inline std::vector<TString> GetActiveVolumes() const { return fActiveVolumes; }

    inline bool GetRemoveUnwantedTracks() const { return fRemoveUnwantedTracks; }

    inline bool GetRemoveUnwantedTracksKeepZeroEnergyTracks() const {
        return fRemoveUnwantedTracksKeepZeroEnergyTracks;
    }

    /// Returns the world magnetic field in Tesla
    inline TVector3 GetMagneticField() const { return fMagneticField; }

    Int_t GetActiveVolumeID(const TString& name);

    Bool_t isVolumeStored(const TString& volume) const;

    void SetActiveVolume(const TString& name, Double_t chance, Double_t maxStep = 0);

    void PrintMetadata() override;

    void Merge(const TRestMetadata&) override;

    TRestGeant4Metadata();
    TRestGeant4Metadata(const char* configFilename, const std::string& name = "");

    ~TRestGeant4Metadata();

    TRestGeant4Metadata(const TRestGeant4Metadata& metadata);
    TRestGeant4Metadata& operator=(const TRestGeant4Metadata& metadata);

    ClassDefOverride(TRestGeant4Metadata, 16);

    // Allow modification of otherwise inaccessible / immutable members that shouldn't be modified by the user
    friend class SteppingAction;
    friend class DetectorConstruction;
    friend class TRestGeant4Hits;
};
#endif  // RestCore_TRestGeant4Metadata
