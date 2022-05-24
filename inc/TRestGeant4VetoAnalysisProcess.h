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

#include <TRestGeant4Event.h>
#include <TRestGeant4Metadata.h>

#include "TRestEventProcess.h"

using namespace std;

class TRestGeant4VetoAnalysisProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestGeant4Event input
    TRestGeant4Event* fInputG4Event;  //!
    /// A pointer to the specific TRestGeant4Event output
    TRestGeant4Event* fOutputG4Event;  //!
    /// A pointer to the simulation metadata information accessible to TRestRun
    TRestGeant4Metadata* fG4Metadata;  //!

    std::vector<int> fVetoVolumeIds;                                        //!
    std::string fVetoKeyword = "";                                          //!
    std::vector<std::string> fVetoGroupKeywords;                            //!
    std::map<std::string, std::vector<std::string>> fVetoGroupVolumeNames;  //!
    std::vector<Float_t> fQuenchingFactors;                                 //!

    void InitFromConfigFile() override;
    void Initialize() override;
    void LoadDefaultConfig();

    // clean std::string (https://stackoverflow.com/questions/216823/whats-the-best-way-to-trim-stdstring)
    inline std::string& rtrim(std::string& s, const char* t = " \t\n\r\f\v") {
        s.erase(s.find_last_not_of(t) + 1);
        return s;
    }
    // trim from beginning of std::string (left)
    inline std::string& ltrim(std::string& s, const char* t = " \t\n\r\f\v") {
        s.erase(0, s.find_first_not_of(t));
        return s;
    }
    // trim from both ends of std::string (right then left)
    inline std::string& trim(std::string& s, const char* t = " \t\n\r\f\v") { return ltrim(rtrim(s, t), t); }

    // final clean std::string: trim and UPPER
    inline std::string& clean_string(std::string& s) {
        s = trim(s);
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    }

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() const override { return fInputG4Event; }
    any GetOutputEvent() const override { return fOutputG4Event; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {
        BeginPrintProcess();

        RESTDebug << "VETO KEYWORD: " << fVetoKeyword << RESTendl;
        RESTDebug << RESTendl;

        RESTDebug << "VETO GROUP KEYWORDS:" << RESTendl;
        for (unsigned int i = 0; i < fVetoGroupKeywords.size(); i++) {
            RESTDebug << "\t" << fVetoGroupKeywords[i] << RESTendl;
        }
        RESTDebug << RESTendl;

        RESTDebug << "Found " << fVetoVolumeIds.size() << " veto volumes:" << RESTendl;
        for (unsigned int i = 0; i < fVetoVolumeIds.size(); i++) {
            RESTDebug << "\t" << fG4Metadata->GetActiveVolumeName(fVetoVolumeIds[i]) << RESTendl;
        }
        RESTDebug << RESTendl;

        RESTDebug << "GROUPS:" << RESTendl;
        for (const auto& pair : fVetoGroupVolumeNames) {
            RESTDebug << "GROUP " << pair.first << " (" << pair.second.size() << " volumes):\n";
            for (unsigned int i = 0; i < pair.second.size(); i++) {
                RESTDebug << "\t" << pair.second[i] << RESTendl;
            }
        }
        RESTDebug << RESTendl;

        RESTDebug << "QUENCHING FACTORS (" << fQuenchingFactors.size() << " Total)" << RESTendl;
        for (unsigned int i = 0; i < fQuenchingFactors.size(); i++) {
            RESTDebug << "\t" << fQuenchingFactors[i] << RESTendl;
        }
        RESTDebug << RESTendl;

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestGeant4VetoAnalysisProcess; }
    /// Returns the name of this process
    const char* GetProcessName() const override { return "geant4VetoAnalysis"; }

    TRestGeant4VetoAnalysisProcess();
    TRestGeant4VetoAnalysisProcess(const char* configFilename);
    ~TRestGeant4VetoAnalysisProcess();

    ClassDefOverride(TRestGeant4VetoAnalysisProcess, 1);
};
#endif  // RestCore_TRestGeant4VetoAnalysisProcess
