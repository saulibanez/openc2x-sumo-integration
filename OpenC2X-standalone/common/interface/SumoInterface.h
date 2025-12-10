// This file is part of OpenC2X.
//
// OpenC2X is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenC2X is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with OpenC2X.  If not, see <http://www.gnu.org/licenses/>.
//
// author: 
// Saul Ibanez <saulibanezcerro@ouc.edu>

#pragma once

#include <string>
#include <iostream>
#include <memory>
#include <map>

#include <utils/traci/TraCIAPI.h>
#include <libsumo/TraCIConstants.h>
#include <libsumo/libsumo.h>

#include <common/utility/CommunicationSender.h>

namespace openc2x {

class SumoInterface {
public:
    SumoInterface(const std::string& host = "127.0.0.1", int port = 9999);
    ~SumoInterface();

    bool connectToSumo();
    bool getVehiclePosition(const std::string& vehID, double& lat, double& lon, double& angle);
    void metersToWGS84(double x, double y, double& lat, double& lon);
    void simulationStep();
    void close();
    
    bool loadVehicleIDFromSumoConfig(const std::string &sumoConfigPath);
    std::string getDefaultVehicleID() const;
    bool isSimulationRunning();

    std::vector<std::string> getAllVehicleIDs();
    bool getVehiclePositionXY(const std::string &vehID, double &x, double &y);

    // semaforo
    void simulationStepWithV2I(LoggingUtility* logger);
    bool isRedLightAhead();
    void sendRedLightWarning();

    bool hasPendingWarning() const { return m_pendingWarning; }
    void clearPendingWarning() { m_pendingWarning = false; }

private:
    std::string m_host;
    int m_port;
    TraCIAPI m_traci;
    bool m_connected;
    std::string m_vehicleID;

    LoggingUtility* m_Logger;
    bool is_logger = false;
    bool m_warningSent = false;
    bool m_pendingWarning = false;
};

} // namespace openc2x
