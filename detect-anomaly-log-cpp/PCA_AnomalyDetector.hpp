#include <Eigen/Dense>

using namespace Eigen;
using namespace std;

class PCA {
private:
	double m_varianceRadio;

public:
	MatrixXd m_projC;
	MatrixXd m_components;
	int m_n_components;
	double m_threshold;
	double m_c_alpha;

    PCA(double n_comp = 0.95, double thres = -1, double alpha = 3.2905)
        : m_n_components(-1), m_threshold(thres), m_c_alpha(alpha) {
        if (n_comp >= 1)
            m_n_components = static_cast<int>(n_comp);
        else
            m_varianceRadio = n_comp;
    }

	void Fit(const MatrixXd &x);

	VectorXi Predict(const MatrixXd &x);
};
