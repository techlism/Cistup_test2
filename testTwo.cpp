#include<iostream>
#include<bits/stdc++.h>
#include <fstream>
#include <sstream>
#include <omp.h>
#include<cmath>
#include"csv.h"
#define TD trajectoryData
using namespace std;

class Trajectory{
public:
    double lati;
    double longi;
    double altitude;
    string date;
    string time;
    long long individualId;
    long long trajectoryId;
    Trajectory(double lati, double longi, double altitude, string date,
     string time, long long individualId, long long trajectoryId){
        this->longi = longi;
        this->lati = lati;
        this->altitude = altitude;
        this->date = date;
        this->individualId = individualId;
        this->trajectoryId = trajectoryId;
        this->time = time;
    }
};
class Coordinates{
    public:
    double lati;
    double longi;
    double altitude;
    Coordinates(double lati,double longi,double altitude){
        this->longi = longi;
        this->lati = lati;
        this->altitude = altitude;        
    }
};
typedef class Trajectory Trajectory;
typedef class Coordinates Coordinates;

double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

double distanceBetween(double lat1, double long1, double alt1, double lat2, double long2, double alt2) {
    const double R = 6371; // Earth's radius in kilometers
    const double dLat = (lat2 - lat1) * M_PI / 180; // delta latitude in radians
    const double dLong = (long2 - long1) * M_PI / 180; // delta longitude in radians
    const double a = pow(sin(dLat / 2), 2) + cos(lat1 * M_PI / 180) * cos(lat2 * M_PI / 180) * pow(sin(dLong / 2), 2);
    const double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    const double d = R * c; // distance in kilometers
    const double h = abs(alt2 - alt1); // delta altitude in meters
    return sqrt(d * d + h * h) / 1000; // return distance in kilometers
}

void calculateTotalDistance(const multimap<long long, Coordinates>& trajMap, map<long long, double>& totalDistanceByIndividual) {
    cout<<"Individual ID    Distance"<<endl;
    #pragma omp parallel for
    for(auto & it : trajMap ){
        auto range = trajMap.equal_range(it.first);
        double distance = 0.0;
        for(auto itr = range.first;itr!=range.second;itr++){
            auto coords1=itr->second;
            auto coords2=next(itr)->second;
            #pragma omp critical
            distance+=distanceBetween(coords1.lati,coords1.longi,coords1.altitude,coords2.lati,coords2.longi,coords2.altitude);
        }
        #pragma omp critical
        totalDistanceByIndividual[it.first]=distance;
        cout<<it.first<<"  "<<distance<<endl;
    }
}




int main(){
    vector<Trajectory> trajectoryData;
    multimap<long long, Coordinates> trajMap;    
    io::CSVReader<7> in("combined_trajectories_copy.csv");
    in.read_header(io::ignore_extra_column, "latitude", "longitude", "altitude", "date", "time", "individual_id", "trajectory_id");
    double latitude, longitude, altitude;
    string date, time;
    long long individualId, trajectoryId;
    while (in.read_row(latitude, longitude, altitude, date, time, individualId, trajectoryId)) {
        Trajectory temp(latitude, longitude, altitude, date, time, individualId, trajectoryId);
        trajectoryData.emplace_back(temp);
        trajMap.emplace(temp.individualId,Coordinates(temp.lati,temp.longi,temp.altitude));
    }
    cout<<"Parsing Complete"<<endl;
    map<long long,double>individualtoDistance;
    calculateTotalDistance(trajMap,individualtoDistance);
    cout<<"Compilation Complete"<<endl;
}
