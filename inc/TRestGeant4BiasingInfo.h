
#ifndef REST_TRESTGEANT4BIASINGINFO_H
#define REST_TRESTGEANT4BIASINGINFO_H

#include <TVector3.h>
#include <set>
#include <string>

class TRestGeant4BiasingInfo {
ClassDef(TRestGeant4BiasingInfo, 1);

private:
    bool fEnabled = false;
    unsigned int fSplittingFactor = 1;
    bool fBiasOncePerTrack = true;
    std::set<std::string> fBiasingVolumes;
    TVector3 fBiasingCenter;

public:
    inline TRestGeant4BiasingInfo() = default;

    inline ~TRestGeant4BiasingInfo() = default;

    inline void SetEnabled(bool enabled) { fEnabled = enabled; }

    inline bool GetEnabled() const { return fEnabled; }

    inline void SetSplittingFactor(unsigned int splittingFactor) { fSplittingFactor = splittingFactor; }

    inline unsigned int GetSplittingFactor() const { return fSplittingFactor; }

    inline void SetBiasOncePerTrack(bool biasOncePerTrack) { fBiasOncePerTrack = biasOncePerTrack; }

    inline bool GetBiasOncePerTrack() const { return fBiasOncePerTrack; }

    inline void AddBiasingVolume(const std::string &volumeName) { fBiasingVolumes.insert(volumeName); }

    inline void ClearBiasingVolumes() { fBiasingVolumes.clear(); }

    inline std::set<std::string> GetBiasingVolumes() const { return fBiasingVolumes; }

    inline void SetBiasingCenter(const TVector3 &biasingCenter) { fBiasingCenter = biasingCenter; }

    inline TVector3 GetBiasingCenter() const { return fBiasingCenter; }
};

#endif  // REST_TRESTGEANT4BIASINGINFO_H
