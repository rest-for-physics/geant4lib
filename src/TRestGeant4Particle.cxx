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
/// TRestGeant4Particle is just a container class to store particle properties
///
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2015-July: First concept and implementation
/// \author     Javier Galan
///
/// \class TRestGeant4Particle
///
/// <hr>
///
#include "TRestGeant4Particle.h"

using namespace std;

ClassImp(TRestGeant4Particle);

TRestGeant4Particle::TRestGeant4Particle() {
    // TRestGeant4Particle default constructor
}

TRestGeant4Particle::~TRestGeant4Particle() {
    // TRestGeant4Particle destructor
}

///////////////////////////////////////////////
/// \brief A copy method
///
void TRestGeant4Particle::SetParticle(TRestGeant4Particle particle) {
	fExcitationLevel = particle.GetExcitationLevel();
	fParticleName = particle.GetParticleName();
	fEnergy = particle.GetEnergy();
	fDirection = particle.GetMomentumDirection();
	fOrigin = particle.fOrigin;
}

///////////////////////////////////////////////
/// \brief Prints on screen the details about the Geant4 simulation
/// conditions, stored in TRestGeant4Metadata.
///
void TRestGeant4Particle::Print() const
{
	std::cout << "Particle name : " << GetParticleName() << std::endl;
	std::cout << "Charge : " << GetParticleCharge() << std::endl;
	std::cout << "Energy : " << GetEnergy() << " keV" << std::endl;
	std::cout << "Excitation level : " << GetExcitationLevel() << std::endl;
	std::cout << "X : " << GetOrigin().X() << "mm Y : " << GetOrigin().Y() << "mm Z : " << GetOrigin().Z() << "mm" << std::endl;
	std::cout << "Px : " << GetMomentumDirection().X() << " Py : " << GetMomentumDirection().Y() << " Pz : " << GetMomentumDirection().Z() << std::endl;
	std::cout << " ---------------------- " << std::endl;
}
