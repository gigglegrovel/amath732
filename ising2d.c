#include <stdio.h>   // printf(), fprintf(), file I/O
#include <stdlib.h>  // rand(), srand(), malloc(), free()
#include <math.h>    // exp(), log(), sqrt()
#include <time.h>    // time(), for random seed


  int lx = 1000;
  int ly = 1000;


// calculate the Hamiltonian
double Hamiltonian(int lattice[lx][ly], double J, double h) {
    double H = 0.0;

    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            int right = lattice[i][(j + 1) % ly];
            int down  = lattice[(i + 1) % lx][j];
            H -= J * lattice[i][j] * (right + down);
            H -= h * lattice[i][j];
        }
    }

    return H;
}
//montecarlo move through pass by reference
void montecarlo(int lattice[lx][ly], double J, double h, double T) {

    // Select random lattice site
    int randx = rand() % lx;
    int randy = rand() % ly;

    // Periodic boundary conditions
    int up    = lattice[(randx - 1 + lx) % lx][randy];
    int down  = lattice[(randx + 1) % lx][randy];
    int left  = lattice[randx][(randy - 1 + ly) % ly];
    int right = lattice[randx][(randy + 1) % ly];

    // Energy difference if spin is flipped
    double del_E = 2.0 * lattice[randx][randy] *
                   (J * (up + down + left + right) + h);

    // Metropolis acceptance criterion
    if (del_E <= 0.0 || (double)rand() / RAND_MAX < exp(-del_E / T)) {
        lattice[randx][randy] *= -1;
    }
}
void save_configuration(int lattice[lx][ly], FILE *video) {

    unsigned char frame[lx][ly];

    for (int i = 0; i < lx; i++) {
        for (int j = 0; j < ly; j++) {
            frame[i][j] = lattice[i][j] > 0 ? 255 : 0;
        }
    }

    fwrite(frame, sizeof(unsigned char), lx * ly, video);
}

int main(void) {

    srand(time(NULL));
       srand(time(NULL));

    // Declare simulation parameters
    int equilibration_sweeps;
    int simulation_sweeps;
    int sampling_freq;

    double J, h, T;

    // Open input file
    FILE *input = fopen("input", "r");

    if (input == NULL) {
        perror("Error opening input file");
        return 1;
    }

    // Read simulation parameters
    int count = fscanf(input,
        "equilibration_sweeps %d\n"
        "simulation_sweeps %d\n"
        "sampling_freq %d\n"
        "J %lf\n"
        "h %lf\n"
        "T %lf",
        &equilibration_sweeps,
        &simulation_sweeps,
        &sampling_freq,
        &J,
        &h,
        &T
    );

    fclose(input);

    if (count != 6) {
        fprintf(stderr, "Error reading input parameters\n");
        return 1;
    }

    if (sampling_freq <= 0 || T <= 0.0 ||
        equilibration_sweeps < 0 || simulation_sweeps <= 0) {
        fprintf(stderr, "Invalid simulation parameters\n");
        return 1;
    }

    long long N = (long long)lx * ly;
    
    // Allocate lattice on the heap
    int (*Lattice)[ly] = malloc(lx * sizeof(*Lattice));

    if (Lattice == NULL) {
        perror("Allocation failed");
        return 1;
    }

    // Initialize lattice
for (int i = 0; i < lx; i++) {
    for (int j = 0; j < ly; j++) {
        Lattice[i][j] = (i + j) % 2 == 0 ? 1 : -1;
    }
}

    // Open video stream
    char command[512];

    snprintf(command, sizeof(command),
        "ffmpeg -y -f rawvideo "
        "-pixel_format gray "
        "-video_size %dx%d "
        "-framerate 30 "
        "-i - "
        "-c:v libx264 "
        "-preset ultrafast "
        "-crf 18 "
        "-pix_fmt yuv420p "
        "ising_movie.mp4",
        ly, lx
    );

    FILE *video = popen(command, "w");

    if (video == NULL) {
        perror("Error opening video stream");
        free(Lattice);
        return 1;
    }

    // Equilibrate the system
    for (int sweep = 0; sweep < equilibration_sweeps; sweep++) {

        for (long long i = 0; i < N; i++) {
            montecarlo(Lattice, J, h, T);
        }

        printf("Equilibration sweep %d\n", sweep + 1);
    }

    // Run simulation
    for (int sweep = 0; sweep < simulation_sweeps; sweep++) {

        for (long long i = 0; i < N; i++) {
            montecarlo(Lattice, J, h, T);
        }

        // Sample configuration
        if ((sweep + 1) % sampling_freq == 0) {
            printf("Sampling sweep %d\n", sweep + 1);
            save_configuration(Lattice, video);
        }
    }

    // Close video stream
    int status = pclose(video);

    free(Lattice);

    return status == 0 ? 0 : 1;
}
