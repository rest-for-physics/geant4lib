/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2020 GIFNA/TREX (University of Zaragoza)                *
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
#include <TRestGeant4Event.h>

#include "TRestGeant4Metadata.h"

class TRestGeant4QuenchingProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestGeant4Event input
    TRestGeant4Event* fInputG4Event;  //!
    /// A pointer to the specific TRestGeant4Event output
    TRestGeant4Event* fOutputG4Event;  //!

    Double_t fQuenchingFactor = 1.0;
    std::set<TString> fParticlesNotQuenched = {"gamma", "e-", "e-", "mu-", "mu+"};

    void InitFromConfigFile() override;
    void Initialize() override;
    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() const override { return fInputG4Event; }
    any GetOutputEvent() const override { return fOutputG4Event; }

    void SetQuenchingFactor(Double_t quenchingFactor);

    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {
        BeginPrintProcess();

        RESTInfo << "Quenching factor: " << fQuenchingFactor << RESTendl;
        RESTInfo << "Particles not quenched: " << RESTendl;

        for (const auto& particle : fParticlesNotQuenched) {
            RESTInfo << "\t" << particle << RESTendl;
        }

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestGeant4QuenchingProcess; }
    /// Returns the name of this process
    const char* GetProcessName() const override { return "geant4QuenchingProcess"; }

    TRestGeant4QuenchingProcess();
    TRestGeant4QuenchingProcess(const char* configFilename);
    ~TRestGeant4QuenchingProcess();

    ClassDefOverride(TRestGeant4QuenchingProcess, 1);
};
#endif  // RestCore_TRestGeant4QuenchingProcess
