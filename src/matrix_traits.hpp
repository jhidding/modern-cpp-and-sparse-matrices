// ~/~ begin <<docs/eigen.md#src/matrix_traits.hpp>>[init]
#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseQR>

namespace ls_bench {
    using Eigen::MatrixXd;
    using Eigen::VectorXd;

    // ~/~ begin <<docs/eigen.md#matrix-traits>>[init]
    struct Sparse {};
    struct Dense {};
    
    template <typename T>
    struct MatrixTraits {};
    // ~/~ end
    // ~/~ begin <<docs/eigen.md#matrix-traits>>[1]
    template <>
    struct MatrixTraits<Dense> {
        typedef Eigen::MatrixXd MatrixType;
    
        inline static void set_element(MatrixType &A, unsigned i, unsigned j, double value) {
            A(i, j) = value;
        }
    
        inline static void make_compressed(MatrixType &A) {}
    
        inline static VectorXd solve_qr(MatrixType const &A, VectorXd const &b) {
            Eigen::HouseholderQR<MatrixXd> direct_solver_qr(A);
            return direct_solver_qr.solve(b);
        }
    };
    // ~/~ end
    // ~/~ begin <<docs/eigen.md#matrix-traits>>[2]
    template <>
    struct MatrixTraits<Sparse> {
        typedef Eigen::SparseMatrix<double> MatrixType;
    
        inline static void set_element(MatrixType &A, unsigned i, unsigned j, double value) {
            A.insert(i, j) = value;
        }
    
        inline static void make_compressed(MatrixType &A) {
            A.makeCompressed();
        }
    
        inline static VectorXd solve_qr(MatrixType const &A, VectorXd const &b) {
            Eigen::SparseQR<MatrixType, Eigen::COLAMDOrdering<int>> direct_solver_qr(A);
            return direct_solver_qr.solve(b);
        }
    };
    // ~/~ end
}
// ~/~ end
