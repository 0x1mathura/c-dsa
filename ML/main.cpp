
#include "lib/SimpleMatrix.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void weightInitialization(smat::Matrix<double>& weights,
				int rows, int cols) {

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			weights(i, j) = (rand() % 100) / 100.0;
		}
	}
}

void baisInitialization(smat::Matrix<double>& bais,
				int rows, int cols) {

	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			bais(i, j) = (rand() % 100) / 100.0;
		}
	}
}


int main() {

	// Training data: 4 samples, 3 features
	smat::Matrix<double> trainingDataX(4, 3);

	smat::Matrix<double> trainingLabelsY(4, 1);


	trainingDataX(0, 0) = 1;
	trainingDataX(0, 1) = 2;
	trainingDataX(0, 2) = 3;

	trainingDataX(1, 0) = 2;
	trainingDataX(1, 1) = 3;
	trainingDataX(1, 2) = 4;

	trainingDataX(2, 0) = 3;
	trainingDataX(2, 1) = 4;
	trainingDataX(2, 2) = 5;

	trainingDataX(3, 0) = 4;
	trainingDataX(3, 1) = 5;
	trainingDataX(3, 2) = 6;

	
	trainingLabelsY(0, 0) = 14;
	trainingLabelsY(1, 0) = 20;
	trainingLabelsY(2, 0) = 26;
	trainingLabelsY(3, 0) = 32;

	
	smat::Matrix<double> weights(3, 1);
	smat::Matrix<double> bais(1, 1);

	weightInitialization(weights, 3, 1);
	baisInitialization(bais, 1, 1);

	double learning_rate = 0.001;
	int epochs = 1000;


	for (int epoch = 0; epoch < epochs; epoch++) {

		// Forward propagation
		smat::Matrix<double> y_hat = trainingDataX * weights;

		// Add bias to each prediction
		for (int i = 0; i < 4; i++) {
			y_hat(i, 0) += bais(0, 0);
		}

		
		smat::Matrix<double> E = y_hat - trainingLabelsY;

		
		smat::Matrix<double> loss = E.transpose() * E;

		double current_loss = loss(0, 0);

		
		smat::Matrix<double> gradient = trainingDataX.transpose() * E;

		double bias_gradient = 0;

		for (int i = 0; i < 4; i++) {
			bias_gradient += E(i, 0);
		}

		weights = weights - learning_rate * gradient;

		bais(0, 0) -= learning_rate * bias_gradient;


		if (epoch % 100 == 0) {
			cout << "Epoch: " << epoch
				<< " Loss: " << current_loss << endl;
		}
	}

	cout << "\nFinal weights:\n";
	for (int i = 0; i < 3; i++) {
		cout << weights(i, 0) << endl;
	}

	cout << "\nFinal bias: " << bais(0, 0) << endl;

	return 0;
}
