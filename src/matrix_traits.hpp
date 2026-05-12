// ~/~ begin <<docs/eigen.md#src/matrix_traits.hpp>>[init]
#pragma once
#include <Eigen/Dense>
#include <Eigen/Sparse>
#include <Eigen/SparseQR>

namespace ls_bench {
    using Eigen::MatrixXd;
    using Eigen::VectorXd;

    // ~/~ begin <<docs/eigen.md#matrix-traits>>[init]
    struct StorageClass {};
    struct Sparse: public StorageClass {};
    struct Dense: public StorageClass {};
    
    struct SolverClass {};
    struct QR: public SolverClass {};
    
    template <typename Real, typename Storage>
    struct MatrixTraits {};
    
    template <typename Matrix, typename Solver>
    struct SolverTraits {};
    // ~/~ end
    // ~/~ begin <<docs/eigen.md#matrix-traits>>[1]
    template <>
    struct MatrixTraits<double, Dense> {
        typedef Eigen::MatrixXd MatrixType;
    
        inline static void set_element(MatrixType &A, unsigned i, unsigned j, double value) {
            A(i, j) = value;
        }
    
        inline static void make_compressed(MatrixType &A) {}
    };
    
    template <>
    struct SolverTraits<MatrixTraits<double, Dense>, QR> {
        using MatrixType = typename MatrixTraits<double, Dense>::MatrixType;
        using SolverType = Eigen::HouseholderQR<MatrixType>;
    
        static SolverType make_solver(MatrixType const &A) {
            return SolverType(A);
        }
    
        inline static VectorXd solve(MatrixType const &A, VectorXd const &b) {
            Eigen::HouseholderQR<MatrixXd> direct_solver_qr(A);
            return direct_solver_qr.solve(b);
        }
    };
    // ~/~ end
    // ~/~ begin <<docs/eigen.md#matrix-traits>>[2]
    template <typename Real>
    struct MatrixTraits<Real, Sparse> {
        typedef Eigen::SparseMatrix<Real> MatrixType;
    
        inline static void set_element(MatrixType &A, unsigned i, unsigned j, double value) {
            A.insert(i, j) = value;
        }
    
        inline static void make_compressed(MatrixType &A) {
            A.makeCompressed();
        }
    };
    
    template <typename Real>
    struct SolverTraits<MatrixTraits<Real, Sparse>, QR> {
        using MatrixType = typename MatrixTraits<Real, Sparse>::MatrixType;
        using SolverType = Eigen::SparseQR<MatrixType, Eigen::COLAMDOrdering<int>>;
    
        static SolverType make_solver(MatrixType const &A) {
            return SolverType(A);
        }
    
        inline static VectorXd solve(MatrixType const &A, VectorXd const &b) {
            SolverType direct_solver_qr(A);
            return direct_solver_qr.solve(b);
        }
    };
    // ~/~ end
}
// ~/~ end
