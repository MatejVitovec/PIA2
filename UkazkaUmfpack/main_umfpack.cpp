#include "Field.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <exception>

extern "C" {
#include "solver_umfpack.h"
}

void source(Field<double>& u, double h) {
    for (int i=0; i<u.size_x(); ++i) {
		for (int j=0; j<u.size_y(); ++j) {
            double x = i*h;
            double y = j*h;
			u(i,j) = 50.0*((x-0.5)*(x-0.5) + (y-0.5)*(y-0.5) < 0.04);
		}
	}
}

void zero(Field<double>& u) {
    for (int i=0; i<u.size_x(); ++i) {
		for (int j=0; j<u.size_y(); ++j) {
			u(i,j) = 0.0;
		}
	}
}

void BC(Field<double>& u) {
    for (int j=0; j<u.size_y(); ++j) {
		u(0,j) = 0.0;
		u(u.size_x()-1,j) = 0.0;
	}
    for (int i=0; i<u.size_x(); ++i) {
		u(i,0) = 0.0;
		u(i,u.size_y()-1) = 0.0;
	}
}

void output(Field<double>& u) {
    std::ofstream outfile("output.dat");
	for (int i=0; i<u.size_x(); ++i) {
		for (int j=0; j<u.size_y(); ++j) {
			outfile << u(i,j) << " ";
	    }
        outfile << "\n";
    }
}

int main() {
	constexpr int N = 500;

    double h = 1.0/double(N);
	
	auto f = Field<double>(N+1,N+1);
    auto u = Field<double>(N+1,N+1);

    source(f,h);
	zero(u);
    BC(u);

    auto b = h*h*f;

	auto stop1 = std::chrono::high_resolution_clock::now();

    solve(u.flatten(),b.flatten(),N);

	//std::cout << "Field max = " << u.max() << "\n";

	auto stop2 = std::chrono::high_resolution_clock::now();

	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(stop2 - stop1).count() << " ms\n";

    output(u);

	return 0;
}