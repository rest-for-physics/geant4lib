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

#ifndef RestCore_TRestGeant4QuenchingProcess
#define RestCore_TRestGeant4QuenchingProcess

#include <TRestEventProcess.h>

#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"

//! Recomputes the energy of every hit based on quenching factor for each particle and volume
class TRestGeant4QuenchingProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestGeant4Event input
    TRestGeant4Event* fInputG4Event;  //!

    /// A pointer to the specific TRestGeant4Event output
    TRestGeant4Event* fOutputG4Event;  //!

    /// A pointer to the simulation metadata information accessible to TRestRun
    TRestGeant4Metadata* fGeant4Metadata;  //!

    /// It stores the quenching factor for each particle for each user volume expression
    std::map<std::string, std::map<std::string, Double_t>> fUserVolumeParticleQuenchingFactor = {};

    /// It stores the quenching factor for each particle for each volume of the simulation
    std::map<std::string, std::map<std::string, Double_t>> fVolumeParticleQuenchingFactor = {};  //!

    void Initialize() override;
    void InitFromConfigFile() override;
    void LoadDefaultConfig();

   public:
    std::vector<std::string> GetUserVolumeExpressions() const;
    Double_t GetQuenchingFactorForVolumeAndParticle(const std::string& volumeName,
                                                    const std::string& particleName) const;

    //
    any GetInputEvent() const override { return fInputG4Event; }
    any GetOutputEvent() const override { return fOutputG4Event; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override;

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestGeant4QuenchingProcess; }

    /// Returns the name of this process
    const char* GetProcessName() const override { return "Geant4Quenching"; }

    TRestGeant4QuenchingProcess();
    explicit TRestGeant4QuenchingProcess(const char* configFilename);
    ~TRestGeant4QuenchingProcess() override;

    ClassDefOverride(TRestGeant4QuenchingProcess, 1);
};
#endif