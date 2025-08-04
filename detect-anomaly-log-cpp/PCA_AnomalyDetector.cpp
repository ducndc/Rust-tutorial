#include "PCA_AnomalyDetector.hpp"

#include <cmath>
#include <iostream>

using namespace Eigen;
using namespace std;

void 
PCA::Fit(
	const MatrixXd &X)
{
	int n = X.rows();
	int d = X.cols();
	MatrixXd X_cov = X.transpose() * X / n;
	SelfAdjointEigenSolver<MatrixXd> eig(X_cov);
	VectorXd sigma = eig.eigenvalues().reverse();
	MatrixXd U = eig.eigenvectors().rowwise().reverse();
	int k = m_n_components;

	if (k < 1) 
	{
		double total = sigma.sum();
		double var = 0.0;

		for (int i = 0; i < d; ++i) 
		{
			var += sigma[i];
			if (var / total >= m_varianceRadio)
			{
				k = i + 1;
				break;
			}
		}
	}

	MatrixXd P = U.leftCols(k);
	MatrixXd I = MatrixXd::Identity(d, d);
	m_projC = I - P * P.transpose();
	m_components = P;

	if (m_threshold < 0) 
	{
		Vector3d phi(0, 0, 0);

		for (int i = k; i < d; ++i)
		{
			phi[0] += sigma[i];
			phi[1] += pow(sigma[i], 2);
			phi[2] += pow(sigma[i], 3);
		}

		double h0 = 1.0 - 2.0 * phi[0] * phi[2] / (3.0 * phi[1] * phi[1]);
		m_threshold = phi[0] * pow(m_c_alpha * sqrt(2.0 * phi[1] * h0 * h0) / phi[0] + 1.0 
					  + phi[1] * h0 * (h0 - 1.0) / (phi[0] * phi[0]), 1.0 / h0);
	}
}

VectorXi 
PCA::Predict(
	const MatrixXd &X)
{
	int n = X.rows();
	VectorXi y_pred = VectorXi::Zero(n);

	for (int i = 0; i < n; ++i)
	{
		VectorXd xa = m_projC * X.row(i).transpose();
		double spe = xa.dot(xa);

		if (spe > m_threshold)
		{
		    y_pred[i] = 1;
		}
	}

	return y_pred;
}

void 
PCA::Evaluate(
	const VectorXi& y_true,
	VectorXi labels) 
{
    cout << "[Evaluation] Evaluating results...\n";
    int TP = 0, FP = 0, FN = 0;

    for (int i = 0; i < labels.size(); ++i) 
    {
        if (labels[i] == 1 && y_true[i] == 1) TP++;
        else if (labels[i] == 1 && y_true[i] == 0) FP++;
        else if (labels[i] == 0 && y_true[i] == 1) FN++;
    }

    double precision = TP / double(TP + FP + 1e-9);
    double recall = TP / double(TP + FN + 1e-9);
    double f1 = 2 * precision * recall / (precision + recall + 1e-9);

    cout << "Precision: " << precision << ", Recall: " << recall << ", F1: " << f1 << endl;
}