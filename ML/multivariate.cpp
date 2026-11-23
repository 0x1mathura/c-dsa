#include "lib/SimpleMatrix.h"
#include <iostream>
#include <cstdlib>

using namespace std;

void weightInitialization( smat::Matrix<double>& weights, int rows, int cols) {

	for (int i = 0; i < rows; i++) 
		for (int j = 0; j < cols; j++) 
			weights(i, j) = (rand() % 100) / 100.0;

}

void biasInitialization( smat::Matrix<double>& bias, int rows, int cols) {

	for (int i = 0; i < rows; i++) 
		for (int j = 0; j < cols; j++) 
			bias(i, j) = 0.0;

}

int main() {


	smat::Matrix<double> X(4, 3);	
	smat::Matrix<double> Y(4, 2);	
	smat::Matrix<double> W(3, 2);	
	smat::Matrix<double> B(1, 2);

	double data[4][3] = {
		{1, 2, 3},
		{2, 3, 4},
		{3, 4, 5},
		{4, 5, 6}
	};

	double labels[4][2] = {
		{14, 20},
		{20, 29},
		{26, 38},
		{32, 47}
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			X(i, j) = data[i][j];
		}

		for (int j = 0; j < 2; j++) {
			Y(i, j) = labels[i][j];
		}
	}

	weightInitialization(W, 3, 2);
	biasInitialization(B, 1, 2);

	double learning_rate = 0.001;
	int epochs = 1000;

	for (int epoch = 0; epoch < epochs; epoch++) {

		// Forward propagation
		smat::Matrix<double> Y_hat = X * W;

		// Add bias to every sample
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 2; j++) {
				Y_hat(i, j) += B(0, j);
			}
		}

		// Error
		smat::Matrix<double> E = Y_hat - Y;

		// Loss
		smat::Matrix<double> loss = E.transpose() * E;
		loss = loss / 8.0;

		double current_loss = 0;

		for (int i = 0; i < 2; i++) {
			current_loss += loss(i, i);
		}


		// Weight gradient
		smat::Matrix<double> gradient = X.transpose() * E;

		// Bias gradient
		smat::Matrix<double> bias_gradient(1, 2);

		for (int j = 0; j < 2; j++) {
			bias_gradient(0, j) = 0;

			for (int i = 0; i < 4; i++) {
				bias_gradient(0, j) += E(i, j);
			}
		}

		// Parameter updates
		W = W - learning_rate * gradient;

		B = B - learning_rate * bias_gradient;

		if (epoch % 100 == 0) {
			cout << "Epoch: " << epoch
				 << " Loss: " << current_loss << endl;
		}
	}

	cout << "\nFinal weights:\n";

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 2; j++) {
			cout << W(i, j) << " ";
		}
		cout << endl;
	}

	cout << "\nFinal biases:\n";

	for (int j = 0; j < 2; j++) {
		cout << B(0, j) << " ";
	}

	cout << endl;

	return 0;
}