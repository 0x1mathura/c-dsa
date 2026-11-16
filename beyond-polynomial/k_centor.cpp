
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <sstream>
#include <algorithm>

using namespace std;


vector<pair<double, double>> readPointsFromFile(string filename) {

	vector<pair<double, double>> points;
	ifstream file(filename);
	if (!file.is_open()) {
		cout << "Error opening file!" << endl;
		return points;
	}

	string line;
	getline(file, line);

	stringstream ss(line);
	char ch;
	double x, y;

	while (ss >> ch) {

		if (ch == '(') {
			ss >> x;
			ss >> ch;
			ss >> y;
			ss >> ch;
			points.push_back({x, y});
		}
	}

	file.close();

	return points;
}



double distance(pair<double, double> p1, pair<double, double> p2) {
		double dx = p1.first - p2.first;
		double dy = p1.second - p2.second;
		return sqrt(dx * dx + dy * dy);
}



vector<pair<double, double>> kCenterGreedy(
	vector<pair<double, double>>& points, int k) {

		vector<pair<double, double>> centers;

		int n = points.size();

		if (n == 0 || k <= 0 || k > n) {
			return centers;
		}


		centers.push_back(points[0]);
		vector<double> minDist(n, 1e18);

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


int main() {

	vector<pair<double, double>> points = readPointsFromFile("points.txt");

	if (points.empty()) {
		cout << "No points found!" << endl;
		return 0;
	}

	int k;

	cout << "Enter number of centers (K): ";
	cin >> k;


	vector<pair<double, double>> centers = kCenterGreedy(points, k);


	cout << "\nSelected Centers:\n";
	for (int i = 0; i < centers.size(); i++) {
		cout << "Center " << i + 1 << ": ("
		<< centers[i].first << ", "
		<< centers[i].second << ")" << endl;
	}
	return 0;
}
