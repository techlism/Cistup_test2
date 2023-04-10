#include <fstream>
#include <sstream>
#include<bits/stdc++.h>
#include<set>
#include<chrono>
using namespace std;

class TripData {
public:
    int tripId;
    pair<string,string> startDateTime;
    pair<string,string> endDateTime;
    pair<double,double> startCord;
    pair<double,double> endCord;

    TripData(int id,pair<string,string> start_time,pair<string,string> end_time,pair<double,double> startCords,pair<double,double> endCords) {
        this->tripId=id;
        this->startDateTime=start_time;
        this->endDateTime=end_time;
        this->startCord=startCords;
        this->endCord=endCords;
    }
};
typedef class TripData TripData;

vector <int> durationGap(vector<TripData>& tripdata,int& maxm ,int& minm){
    vector<int>gaps;
    for(auto data:tripdata){
        int h1 = stoi(data.startDateTime.second.substr(0,2));
        int m1 = stoi(data.startDateTime.second.substr(3,2)) + (h1 * 60);
        int h2 = stoi(data.endDateTime.second.substr(0,2));
        int m2 = stoi(data.endDateTime.second.substr(3,2)) + (h2 * 60);  
        int diff = abs(m2-m1);
        if(diff){
            maxm=max(maxm,diff);
            minm=min(minm,diff);
            gaps.push_back(diff);
        }
    }
    sort(gaps.begin(),gaps.end());
    return gaps;
}

int circularTrips(vector<TripData>& tripdata){
    int count = 0 ;
    for(auto data: tripdata){
        if(data.endCord==data.startCord)
        count++;
    }
    return count;
}

vector<pair<double,double>> uniqueDepots(vector<TripData> &tripdata){
    vector<pair<double,double>> unique;
    set<pair<double,double>> uniqueSet;
    for(int i=0;i<100;i++){
        if(uniqueSet.count(tripdata[i].startCord) == 0){
            uniqueSet.insert(tripdata[i].startCord);
            unique.push_back(tripdata[i].startCord);
        }
        if(uniqueSet.count(tripdata[i].endCord) == 0){
            uniqueSet.insert(tripdata[i].endCord);
            unique.push_back(tripdata[i].endCord);
        }
    }
    return unique;
}

double euclideanDistance(pair<double,double> p1, pair<double,double> p2) {
    double dx = p2.first - p1.first;
    double dy = p2.second - p1.second;
    return sqrt(dx*dx + dy*dy);
}

vector<double> uniqueDepotDistances(vector<pair<double,double>> uniqueDepots){
    vector<double>depotDistances;
    for(int i=0;i<uniqueDepots.size();i++){
        for(int j=i+1;j<uniqueDepots.size();j++){
            depotDistances.emplace_back(euclideanDistance(uniqueDepots[i],uniqueDepots[j]));
        }
    }
    sort(depotDistances.begin(),depotDistances.end());
    return depotDistances;
}

void printInfo(vector<TripData> &tripdata){
    int maxm=INT_MIN;
    int minm=INT_MAX;
    vector<int>gaps = durationGap(tripdata,maxm,minm);
    int count=0;
    for(auto g :gaps){
        if(g>minm) break;
        else count++;
    }
    double circularPercent = (circularTrips(tripdata)*100)/tripdata.size();
    cout<<"Maximum duration of the trip (in minutes): "<<maxm<<endl;
    cout<<"Minimum duration of the trip (in minutes): "<<minm<<endl;
    cout<<"Total number of trips corresponding to the minimum duration: "<<count<<endl;    
    cout<<"Percentage of total circular trips: "<<setprecision(5)<<circularPercent<<endl;   
}

vector<pair<TripData,TripData>> feasibleTrips(vector<TripData>& tripdata){
    vector<TripData> filtered;
    for(auto data:tripdata){
        int h1 = stoi(data.startDateTime.second.substr(0,2));
        int m1 = stoi(data.startDateTime.second.substr(3,2)) + (h1 * 60);
        int h2 = stoi(data.endDateTime.second.substr(0,2));
        int m2 = stoi(data.endDateTime.second.substr(3,2)) + (h2 * 60);
        if(m1>=360 && m2<=1080)
            filtered.emplace_back(data);
    }
    
    vector<pair<TripData,TripData>> feasibleTrips;
    for(int i=0;i<filtered.size();i++){
        for(int j=i+1;j<filtered.size();j++){
            int h2a = stoi(filtered[i].endDateTime.second.substr(0,2));
            int m2a = stoi(filtered[i].endDateTime.second.substr(3,2)) + (h2a * 60);
            int h2b = stoi(filtered[j].endDateTime.second.substr(0,2));
            int m2b = stoi(filtered[j].endDateTime.second.substr(3,2)) + (h2b * 60);                        
            if(filtered[i].endCord==filtered[j].startCord && m2b >= m2a){
                feasibleTrips.push_back({filtered[i],filtered[j]});
            }
        }
    }
    
    return feasibleTrips;
}

int main() {
    vector<TripData> tripdata;
    // Opening the CSV file
    ifstream file("bike_data_new.csv");
    // Reading each line from the CSV file
    string line;
    while (getline(file, line)) {
        // Split the line into individual values
        stringstream ss(line);
        string trip_id,started_at,ended_at,start_lat,start_lng,end_lat,end_lng;
        // Extracting each column values
        getline(ss, trip_id, ',');
        getline(ss, started_at, ',');
        getline(ss, ended_at, ',');
        getline(ss, start_lat, ',');
        getline(ss, start_lng, ',');
        getline(ss, end_lat, ',');
        getline(ss, end_lng, ',');
        TripData temp(
            stoi(trip_id),
            {started_at.substr(0,10),started_at.substr(11,5)},
            {ended_at.substr(0,10),ended_at.substr(11,5)},
            {stod(start_lat),stod(start_lng)},
            {stod(end_lat),stod(end_lng)}
        );
        tripdata.emplace_back(temp);        
    }
    file.close();
    auto start = std::chrono::high_resolution_clock::now();
    printInfo(tripdata);
    cout<<"Total feasible trips are: "<<feasibleTrips(tripdata).size()<<endl;
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    cout << "Total Runtime: " << elapsed.count() << " s" <<endl;
    cout<<"Euclidean Distances "<<endl;
    auto s = std::chrono::high_resolution_clock::now();
    for(auto i: uniqueDepotDistances(uniqueDepots(tripdata))) 
        cout <<i<<endl;
    auto e = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapse = e - s;
    cout << "Total Runtime(for Distance Calculation): " << elapse.count() << " s" <<endl;
}
