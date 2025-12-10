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

#include "SumoInterface.h"
#include <libsumo/TraCIDefs.h> 
#include <regex>
#define _USE_MATH_DEFINES
#include <cmath>

namespace openc2x {

SumoInterface::SumoInterface(const std::string& host, int port)
    : m_host(host), m_port(port), m_connected(false){}


SumoInterface::~SumoInterface() 
{
    if (m_connected) {
        try {
            m_traci.close();
            std::cout << "[SumoInterface] Disconnected from SUMO\n";
        } catch (...) {
            std::cerr << "[SumoInterface] Error closing TraCI connection\n";
        }
    }
}

bool SumoInterface::connectToSumo() 
{
    try {
        std::cout << "[SumoInterface] Connecting to SUMO at "
                  << m_host << ":" << m_port << " ...\n";

        m_traci.connect(m_host, m_port);
        m_connected = true;

        auto version = m_traci.getVersion();
        std::cout << "[SumoInterface] Connected. TraCI version "
                  << version.first << " | SUMO " << version.second << "\n";
        return true;
    } catch (const std::exception& e) {
        std::cerr << "[SumoInterface] Connection error: " << e.what() << "\n";
        m_connected = false;
        return false;
    }
}

bool SumoInterface::getVehiclePosition(const std::string& vehID,
                                       double& lat, double& lon, double& alt)
{
    try {
        libsumo::TraCIPosition pos = m_traci.vehicle.getPosition(vehID);
        metersToWGS84(pos.x, pos.y, lat, lon);
        alt = 0.0;
        return true;
    } catch (const tcpip::SocketException& e) {
        std::cerr << "[SumoInterface] Error getting vehicle position: "
                  << e.what() << std::endl;
        return false;
    }
}

static constexpr double WGS84_RADIUS = 6378137.0; 
static constexpr double BASE_LAT = 40.4168 * M_PI / 180.0;
static constexpr double BASE_LON = -3.7038 * M_PI / 180.0;

void SumoInterface::metersToWGS84(double x, double y, double& lat, double& lon)
{
    // Conversion a metros
    double dLat = y / WGS84_RADIUS;
    double dLon = x / (WGS84_RADIUS * cos(BASE_LAT));

    lat = BASE_LAT + dLat;
    lon = BASE_LON + dLon;

    // Convertir a grados
    lat = lat * 180.0 / M_PI;
    lon = lon * 180.0 / M_PI;
}

void SumoInterface::simulationStep() 
{
    try {
        m_traci.simulationStep();

        std::string tls = m_traci.trafficlights.getRedYellowGreenState("A1");
        std::cout << "[V2I] Traffic light A1 state: " << tls << std::endl;

    } catch (const tcpip::SocketException& e) {
        std::cerr << "[SumoInterface] simulationStep error: " << e.what() << std::endl;
    }
}

bool SumoInterface::loadVehicleIDFromSumoConfig(const std::string &sumoConfigPath) 
{
    std::ifstream infile(sumoConfigPath);
    if (!infile.is_open()) {
        std::cerr << "[SumoInterface] Cannot open SUMO route file: " << sumoConfigPath << std::endl;
        return false;
    }

    std::string line;
    std::regex pattern(".*<vehicle.*id=\"([^\"]+)\".*>");
    while (std::getline(infile, line)) {
        std::smatch match;
        if (std::regex_search(line, match, pattern)) {
            m_vehicleID = match[1];
            std::cout << "[SumoInterface] Vehicle: " << m_vehicleID << std::endl;
            return true;
        }
    }

    std::cerr << "[SumoInterface] No vehicle ID found in " << sumoConfigPath << std::endl;
    return false;
}

std::string SumoInterface::getDefaultVehicleID() const 
{
    return m_vehicleID;
}

bool SumoInterface::isSimulationRunning() 
{
    try {
        return m_traci.simulation.getMinExpectedNumber() > 0;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> SumoInterface::getAllVehicleIDs() 
{
    try {
        return m_traci.vehicle.getIDList();
    } catch (...) {
        return {};
    }
}

bool SumoInterface::getVehiclePositionXY(const std::string &vehID,
                                         double &x, double &y)
{
    try {
        auto pos = m_traci.vehicle.getPosition(vehID);
        x = pos.x;
        y = pos.y;
        return true;
    } catch (...) {
        return false;
    }
}

bool SumoInterface::isRedLightAhead()
{
    if (!m_connected)
        return false;

    // Obtener lista de semaforos
    auto tlsIds = m_traci.trafficlights.getIDList();
    if (tlsIds.empty())
        return false;

    // Obtener vehiculo
    auto vehs = m_traci.vehicle.getIDList();
    if (vehs.empty())
        return false;

    std::string vehId = vehs[0];
    auto vehPos = m_traci.vehicle.getPosition(vehId);

    const double maxDist = 25.0;

    // Recorrer todos los semaforos
    for (const auto& tlID : tlsIds)
    {
        // Estado actual ("rrrGGg...")
        std::string state = m_traci.trafficlights.getRedYellowGreenState(tlID);
        bool isRed = (state.find('r') != std::string::npos);

        // Obtener posicion del cruce asociado al semaforo
        auto tlPos = m_traci.junction.getPosition(tlID);

        double dx = vehPos.x - tlPos.x;
        double dy = vehPos.y - tlPos.y;
        double dist = std::sqrt(dx*dx + dy*dy);

        std::cout << "[V2I] TL " << tlID
                  << " state=" << state
                  << " dist=" << dist << std::endl;

        if (isRed && dist < maxDist)
        {
            std::cout << "[V2I] WARNING: Red light detected at " << tlID << "\n";
            m_pendingWarning = true;
            return true;
        }
    }

    return false;
}

void SumoInterface::sendRedLightWarning()
{
    m_pendingWarning = true;
}

void SumoInterface::simulationStepWithV2I(LoggingUtility* logger)
{
    try {
        if (!is_logger) {
            m_Logger = logger;
            is_logger = true;
        }

        m_traci.simulationStep();
    }
    catch (const tcpip::SocketException& e) {
        std::cerr << "[SumoInterface] simulationStep error: "
                  << e.what() << std::endl;
        return;
    }

    // ---------- LOGICA V2I ----------
    if (isRedLightAhead()) {
        if (!m_warningSent) {
            m_warningSent = true;
            sendRedLightWarning();
            std::cout << "[V2I] Semaforo en rojo -> aviso pendiente\n";
        }

    } else {
        m_warningSent = false;
    }
}

} // namespace openc2x
