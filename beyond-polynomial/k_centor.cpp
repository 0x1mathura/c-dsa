
#include <climits>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <chrono>
#include "lib/rapidcsv.h"

using namespace std;

vector<tuple<string, double, double>> readPointsFromFile(string filename) {
	vector<tuple<string, double, double>> points;
	ifstream file(filename);

	string location;
	double x, y;
	char ch;

	while (file >> ch) {
		if (ch != '(') continue;

		getline(file, location, ',');
		file >> ch >> x >> ch >> y >> ch >> ch;
		points.push_back({location, x, y});
	}

	return points;
}


vector<tuple<string, double, double>> readPointsFromFile_earthquakes(string filename) {
	vector<tuple<string, double, double>> points;
	ifstream file(filename);	
	string location;
	double x, y;
	char ch;

	while (file >> ch) {
		if (ch != '(') continue;

		getline(file, location, '(');
		file >> x >> ch >> y >> ch >> ch;

		points.push_back({location, x, y});
	}
	return points;
}


double distance(tuple<string, double, double> p1, tuple<string, double, double> p2) {
		double dx = get<1>(p1) - get<1>(p2);
		double dy = get<2>(p1) - get<2>(p2);
		return sqrt(dx * dx + dy * dy);
}



vector<tuple<string, double, double>> kCenterGreedy(
	vector<tuple<string, double, double>>& points, int k) {

		vector<tuple<string, double, double>> centers;

		int n = points.size();

		if (n == 0 || k <= 0 || k > n) {
			return centers;
		}


		centers.push_back(points[0]);
		vector<double> minDist(n, INT_MAX);

		for (int i = 0; i < n; i++) {
			minDist[i] = distance(points[i], centers[0]);
		}
		for (int count = 1; count < k; count++) {
			int farthestIndex = -1;
			double maxDist = -1;

			for (int i = 0; i < n; i++) {
				if (minDist[i] > maxDist) {
					maxDist = minDist[i];
					farthestIndex = i;
				}
			}
			centers.push_back(points[farthestIndex]);


			for (int i = 0; i < n; i++) {
				double d = distance(points[i],points[farthestIndex]);
				minDist[i] = min(minDist[i], d);
			}
		}
	return centers;
}


	bool prepare_csv_file(string result_file){

		vector<string>column_name={"data_type","no_of_data_points","no_of_centor(k)","time_taken(milli_second)"};

		rapidcsv::Document doc(result_file);
			
		for (int i=0;i<column_name.size();i++){
			doc.SetColumnName(i , column_name[i]);

		}
		doc.Save();

		return true;
	}
	void add_entry_result(int index,string result_file, string data_set_name,vector<string>&row){
		
		rapidcsv::Document doc(result_file);
		doc.InsertRow(index,row);
		doc.Save();
		
	}


int main() {

	vector<string>data_type={"toy_points.txt","dns.txt","earthquakes.txt","healthcare.txt","poi.txt","trees.txt"};
	vector<string>data_type_names={"toy_points","public dns server in internet","earthquakes in USA","healthcare facilities in India","points of interest in USA","trees in USA philadelphia"};
	string resultfile="k_centor.csv";
	fstream result(resultfile, ios::app);
	prepare_csv_file(resultfile);

	for (int i = 0; i < data_type.size(); i++) {
		auto start = chrono::high_resolution_clock::now();
		vector<tuple<string, double, double>> points;
		if (data_type[i] == "earthquakes.txt") {
			points = readPointsFromFile_earthquakes("./kcentor_dataset/cleaned_data/" + data_type[i]);
		} else {
			points = readPointsFromFile("./kcentor_dataset/cleaned_data/" + data_type[i]);
		}
		int k;
		k=points.size()/10;
		cout<<"data type: "<<data_type[i]<<" points: "<<points.size()<<" k: "<<k<<endl;

		vector<tuple<string, double, double>> centers = kCenterGreedy(points, k);
		auto end = chrono::high_resolution_clock::now();
		chrono::duration<double, milli> time_taken = end - start;

		vector<string>result_row;

		result_row.push_back(data_type_names[i]);
		result_row.push_back(to_string(points.size()));
		result_row.push_back(to_string(k));
		result_row.push_back(to_string(time_taken.count()));
		add_entry_result(i, resultfile, data_type_names[i], result_row);
		cout << "\ncompleted datatype: " << data_type_names[i] << "\n";

	}
	return 0;
}
