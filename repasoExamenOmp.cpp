#include <omp.h>
#include <vector>
#include <fmt/core.h>
#include <iostream>
#include <fstream>

std::vector<int> datos;


std::vector<int> read_file() {
    std::fstream fs("C:/Users/user/CLionProjects/examen/datos.txt", std::ios::in);
    std::string line;
    std::vector<int> ret;
    while (std::getline(fs, line)) {
        ret.push_back(std::stoi(line));
    }
    fs.close();
    return ret;
}

std::vector<int> serial_frecuencias() {

    std::vector<int> cc(101);
    for (int dato : datos) {
        cc[dato]++;
    }
    return cc;
}

std::vector<int> frecuencias_paralela(){

    std::vector<int> cc(101);
    int block_size;
    int num_threads;
    int tamanio = datos.size();

#pragma omp parallel default(none) shared(cc, num_threads, block_size, tamanio, datos)
    {
#pragma omp master
        {
            num_threads= omp_get_num_threads();
            block_size = tamanio/num_threads;
            fmt::println("Block_size: {}", block_size);
        }

        int thread_id= omp_get_thread_num();
        int start = thread_id * block_size;
        int end = (thread_id+1) * block_size;

        std::vector<int> cc_local(101);
        for(int i = start; i < end; i++){
            cc_local[datos[i]]++;
        }

#pragma omp critical
        {

            for (int i = 0; i < cc.size() ; ++i) {
                cc[i] = cc[i]+ cc_local[i];
            }
        };



    };

    return cc;


}

std::vector<int> frecuencia_paralela2(){

    std::vector<int> vector(101);

#pragma omp parallel default(none) shared(vector, datos)
    {
        std::vector<int> vector_local(101);

#pragma omp for
        for(int i =0; i<datos.size(); i++){
            vector_local[datos[i]]++;
        }

#pragma omp critical
        {
            for(int i =0; i< 101; i++){
                vector[i] = vector[i] +vector_local[i];

            }
        };

    };

    return vector;
}

double promedio (){

    int suma =0;
    for(int i =0; i<datos.size(); i++){
        suma = suma+ datos[i];
    }

    return suma/(double)datos.size();

}

double promedio_paralelo(){

    int suma_total =0;
    long block_size;


#pragma omp parallel default(none) shared(suma_total, block_size, datos)
    {

        int thread_id = omp_get_thread_num();
        int thread_num = omp_get_num_threads();


        int suma_parcial=0;
        for (int i = thread_id; i < datos.size(); i += thread_num) {
            suma_parcial = suma_parcial+ datos[i];
        }

#pragma omp critical
        {
            suma_total = suma_total + suma_parcial;
        };
    };
    return suma_total / (double )datos.size();
}


double promedio_paralelo2(){

    int suma_total =0;
    long block_size;


#pragma omp parallel default(none) shared(suma_total, block_size, datos)
    {

        int thread_id = omp_get_thread_num();
        int thread_num = omp_get_num_threads();
        block_size = datos.size()/thread_num;
        int start= thread_id * block_size;
        int end = (thread_id+1)* block_size;

        int suma_parcial=0;
        for (int i = start; i < end; i++) {
            suma_parcial = suma_parcial+ datos[i];
        }

#pragma omp critical
        {
            suma_total = suma_total + suma_parcial;
        };
    };
    return suma_total / (double )datos.size();
}



int main(){

    datos = read_file();

        fmt::println("{}",  datos.size() );
        std:: vector<int> data = serial_frecuencias();
        for(int i = 0; i< 101; i++) {
            fmt::println("Numero {}, frecuencia {}", i, data[i]);
        }
    fmt::println("*****************************************************************");
    fmt::println("Calculando frecuencias paralelas");

    std:: vector<int> data1 = frecuencias_paralela();
    for(int i = 0; i< 101; i++) {
        fmt::println("Numero {}, frecuencia {}", i, data1[i]);
    }
    fmt::println("*****************************************************************");
    fmt::println("Calculando frecuencias paralelas 2");
    std:: vector<int> data2 = frecuencia_paralela2();
    for(int i = 0; i< 101; i++) {
        fmt::println("Numero {}, frecuencia {}", i, data2[i]);
    }
    fmt::println("*****************************************************************");

    fmt::println("Calculando promedio");
    double prom = promedio();
    fmt::println("El promedio es: {}", prom);
    fmt::println("*****************************************************************");
    fmt::println("Calculando promedio paralelo");
    double prom1 = promedio_paralelo();
    fmt::println("El promedio es: {}", prom1);

    fmt::println("*****************************************************************");
    fmt::println("Calculando promedio paralelo 2");
    double prom2 = promedio_paralelo2();
    fmt::println("El promedio es: {}", prom2);
}