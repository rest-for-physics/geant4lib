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

#ifndef RestCore_TRestGeant4VetoAnalysisProcess
#define RestCore_TRestGeant4VetoAnalysisProcess

#include <TRestEventProcess.h>

#include <string>

#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"

struct Veto {
    std::string name;
    std::string alias{};
    std::string group{};
    int layer{};
    int number{};
    TVector3 position{};
    double length{};
};

class TRestGeant4VetoAnalysisProcess : public TRestEventProcess {
   private:
    std::string fVetoVolumesExpression;  // identify veto volumes

   public:
    inline std::string GetVetoVolumesExpression() const { return fVetoVolumesExpression; }

   private:
    TRestGeant4Event* fInputEvent = nullptr;               //!
    TRestGeant4Event* fOutputEvent = nullptr;              //!
    const TRestGeant4Metadata* fGeant4Metadata = nullptr;  //!

    std::vector<Veto> fVetoVolumes;

    void InitFromConfigFile() override;
    void Initialize() override;
    void LoadDefaultConfig();

   public:
    any GetInputEvent() const override { return fInputEvent; }
    any GetOutputEvent() const override { return fOutputEvent; }

    static Veto GetVetoFromString(const std::string& vetoString);

    inline void SetGeant4Metadata(const TRestGeant4Metadata* metadata) {
        fGeant4Metadata = metadata;
    }  // TODO: We should not need this! but `GetMetadata<TRestGeant4Metadata>()` is not working early in the
       // processing (look at the tests for more details)

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() override;

    const char* GetProcessName() const override { return "geant4VetoAnalysis"; }

    TRestGeant4VetoAnalysisProcess();
    TRestGeant4VetoAnalysisProcess(const char* configFilename);
    ~TRestGeant4VetoAnalysisProcess();

    ClassDefOverride(TRestGeant4VetoAnalysisProcess, 4);
};
#endif  // RestCore_TRestGeant4VetoAnalysisProcess
