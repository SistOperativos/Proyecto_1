#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
#include <limits.h>

#define TRUE 1
#define FALSE 0
#define LEFT 0
#define RIGHT 1
#define RADIOACTIVE 0
#define URGENT 1
#define NORMAL 2
#define MAX_WEIGHT 10

typedef struct Package{
    int weight;          // Band's weight
    int type;            // Band's type
} Package;

typedef struct State{
    int state;
    Package package;
    int percentage;
}State;

typedef struct Band{
    int band_strength;   //Band Strength
    int band_length;     //Band Length
    int sign_side;       //Current side of the sign
} Band;

typedef struct Array{
    int length;
    Package *package_array;
}Array;




float coefficient_mk = 0.56f;//Friction Kinetic coefficient between leather and metal. mk = 0.56

int genRandoms(int lower, int upper)
{
    int num = (rand() % (upper - lower + 1)) + lower;
    return num;
}



double factorial(int k)
{
    double kf = 1;
    for(int i = 2; i<=k; i++){
        kf= kf*i;
    }
    return kf;
}


int poisson()
{
    //Initializing random generator
    int l = 10;
    int k = genRandoms(0,20);             //k
    double kf= factorial(k);                                     //k!
    double f = ((exp(-l))*(pow(l,k)))/(kf);
    return (int)fabs(f*100);
}

int normalRandom() {
    double u = ((double) rand() / (RAND_MAX)) * 2 - 1;
    double v = ((double) rand() / (RAND_MAX)) * 2 - 1;
    double r = u * u + v * v;
    if (r == 0 || r > 1){
        return normalRandom();
    } else {
        double c = sqrt(-2 * log(r) / r);
        return (int)fabs(u * c*100);
    }
}

Array* package_gen(int distribution, float n_percentage, float u_percentage, float r_percentage){
    int package_num;
    /* //TODO: lineas comentadas para prueba, eliminar
    if(distribution == 0){ //Normal
        package_num = normalRandom();
    } else { //poisson
        package_num = poisson();
    }
     */
    package_num = 10; //TODO: eliminar
    int urgent_num = (int)ceil((u_percentage*(float)package_num));
    int radioactive_num = (int)ceil((r_percentage*(float)package_num));
    int normal_num = (int)ceil((n_percentage*(float)package_num));
    Array* packages_list = malloc(sizeof(Array));
    packages_list->length = urgent_num + radioactive_num + normal_num;
    packages_list->package_array = malloc(packages_list->length* sizeof(Package));
    if(packages_list->package_array == NULL){
        printf("Memory hasn't been allocated correctly \n");
        exit(0);
    }
    int counter = 0;
    while(urgent_num != 0 || radioactive_num != 0 || normal_num != 0){
        int t = genRandoms(0,2);
        if((t == RADIOACTIVE && radioactive_num != 0) || ((urgent_num == 0 || normal_num == 0) && radioactive_num != 0)){ // radioactive
            Package package;
            package.type = RADIOACTIVE;
            package.weight = genRandoms(1, MAX_WEIGHT);
            packages_list->package_array[counter] = package;
            radioactive_num -= 1;
            counter += 1;
            continue;
        } else if ((t == URGENT && urgent_num != 0) || ((radioactive_num == 0 || normal_num == 0) && urgent_num != 0)){
            Package package;
            package.type = URGENT;
            package.weight = genRandoms(1,MAX_WEIGHT);
            packages_list->package_array[counter] = package;
            urgent_num -= 1;
            counter += 1;
            continue;
        } else if((t == NORMAL && normal_num != 0) || ((radioactive_num == 0 || urgent_num == 0) && normal_num != 0)){
            Package package;
            package.type = NORMAL;
            package.weight = genRandoms(1,MAX_WEIGHT);
            packages_list->package_array[counter] = package;
            normal_num -= 1;
            counter += 1;
            continue;
        } else {
            continue;
        }
    }
    return packages_list;

}

int shorter_scheduler(Array* packages, int used_index[],int used_index_lenght)
{
    int chosenOne = 0;
    int found =0;

    for (int i=0; i<packages->length; i++)
    {   
        for(int x =0; x < used_index_lenght;x++) {
            if(i != used_index[x]){
                continue;
            } else {
                found =1;
                break;
            } 
        }
        if(found ==0){
            int weight = packages->package_array[i].weight;
            int chosenOneWeight = packages->package_array[chosenOne].weight;
                if( i = 0 ) {
                    chosenOne = i;
                }
                else if (chosenOneWeight > weight) {
                    chosenOne = i;
                }
        }
        else
            {
                continue;
        }
            
    }
    
    return chosenOne;
}

int priority_scheduler(Array* packages,int used_index[], int used_index_lenght){
    int chosenOne = NULL;
    int secondChoice = NULL;
    int lastChoice = NULL;
    int found = 0;
    for (int i = 0; i<packages->length; i++)
    {
        found = 0;
        for(int x=0; x<used_index_lenght; x++) {
            if(i != used_index[x]){
                continue;
            } else {
                found =1;
                break;
            }  
        }
        if(found == 0){
            int priority = packages->package_array[i].type;
            if( priority == RADIOACTIVE) {
                return i;
            }
            else if (priority == URGENT) {
                secondChoice = i;
            }
            else 
                lastChoice = i;
        }
        else
        {
            continue;
        }
        
    }
    if (secondChoice != NULL){
        return secondChoice;
    }
    else
    {
        return lastChoice;
    }
}


void print_packages(Array* package_list){ // TODO: Borrar esta función, es solo para prueba
    int r = 0;
    int u = 0;
    int n = 0;
    for(int i = 0; i < package_list->length; i++){
        int k = package_list->package_array[i].type;
        int weight = package_list->package_array[i].weight;
        if(k == RADIOACTIVE){
            printf("Radioactivo \n Peso: %d \n", weight);
            r += 1;
        } else if(k == URGENT){
            printf("Urgente \n Peso: %d \n", weight);
            u += 1;
        } else {
            printf("Normal \n Peso: %d \n", weight);
            n += 1;
        }
    }
    printf("Radioactivos: %d \nUrgentes: %d \nNormales: %d \nTotal: %d \n \n", r, u, n, package_list->length);
}

//calculating total time to commute a packages between sides
int get_t_time(int package_weight, int band_length, int band_strength){
    float n = 9.8f*(float)package_weight;  //Calculating normal
    float Fk = coefficient_mk*n;        //Calculating a friction strength
    if((float)band_strength < Fk){
        printf("The band strength isn't enough to carry the package");
        exit(0);
    }
    else{
        float Ft = (float)band_strength - Fk;  //Calculating total strength
        float a = Ft/(float)package_weight;       //Calculating acceleration
        float t_time = sqrtf((2*(float)band_length)/a); //Calculating time
        return (int)ceil(t_time);  // Return rounded time value
    }
}

void package_equity_band(int W, Band *band, State *state, int distribution, float n_percentage, float u_percentage, float r_percentage, int schedulerType){
    while(TRUE){
        int package_counter = 0;   // Save the number of packages that have been passed through the band
        int left_array_index = 0;  // Save the current position into left array
        int right_array_index = 0; // Save the current position into right array 
        Array *left_array = package_gen(distribution, n_percentage, u_percentage, r_percentage);
        Array *right_array = package_gen(distribution, n_percentage, u_percentage, r_percentage);
        int left_index_counter[left_array->length];
        int right_index_counter[right_array->length];
        print_packages(left_array); //TODO: elminar esto
        print_packages(right_array); //TODO: eliminar esto
        int current_side = LEFT;
        int transport_time;
        int pkg_index = 0;


        // Validating that the index are still into the arrays
        while(left_array->length != left_array_index || right_array->length != right_array_index){
            if(current_side == LEFT){ // If the current side is left, we use a package of the left
                if(schedulerType == 1){
                    left_array_index = priority_scheduler(left_array,left_index_counter,left_array->length);
                    left_index_counter[left_array_index] = left_array_index;
                    printf("%d \n",left_index_counter[left_array_index]);
                     
                }
                else if (schedulerType == 2){
                    left_array_index = shorter_scheduler(left_array,left_index_counter, left_array->length);
                    left_index_counter[left_array_index] = left_array_index;
                }
                Package *package = &left_array->package_array[left_array_index]; // Define a package to extract the information
                //printf("%d \n",package->type);
                left_array_index += 1; // Increment the left index 
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
                * package will take to pass the through the band*/
                state->package = *package;  // Defining which package is used at the time
                state->percentage = 0;
            } else { // If the current side is right, we use a package of the right
                if(schedulerType == 1){
                        right_array_index = priority_scheduler(right_array,right_index_counter,right_array->length);
                        right_index_counter[right_array_index] = right_array_index;
                        
                    }
                else if (schedulerType == 2){
                        right_array_index = shorter_scheduler(right_array,right_index_counter,right_array->length);
                        right_index_counter[right_array_index] = right_array_index;
                    } 
                Package *package = &right_array->package_array[right_array_index]; // Define a package to extract the information
                right_array_index += 1; // Increment the left index
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                state->package = *package; // Defining which package is used at the time
                state->percentage = 0;
            }
            for(int i = 0; i < transport_time; i++){ // Loop to simulate the package transport
                state->state = i;                    // Save the current loop's current state
                state->percentage = (int)(((float)(i+1)/(float)transport_time)*100);
                sleep(1);                    // Wait for a second
            }
            //printf("%d \n",current_side); //TODO: Eliminar esto
            if(package_counter == W-1) {             // If it was reached the W value
                if (current_side == LEFT && right_array_index != right_array->length) { /* If the current side is LEFT
            * and wasn't reached the maximum of the right_array*/
                    current_side = RIGHT;  // Changing side
                    package_counter = 0;   // Restarting W counter;
                    continue;
                } else if(current_side == RIGHT && left_array_index != left_array->length){ /* If the current side is RIGHT
            * and wasn't reached the maximum of the left_array*/
                    current_side = LEFT; // Changing side
                    package_counter = 0; // Restarting W counter;
                    continue;
                } else{
                    package_counter = 0; // Restarting W counter;
                    continue;
                }
            } else {
                if(current_side == LEFT && left_array_index == left_array->length){ // If the maximum array value was reached, change side
                    current_side = RIGHT;
                    package_counter = 0;
                    continue;
                } else if(current_side == RIGHT && right_array_index == right_array->length){ // If the maximum array value was reached, change side
                    current_side = LEFT;
                    package_counter = 0;
                    continue;
                } else {
                    package_counter += 1; // Increment W counter;
                    continue;
                }
            }
        }
    }
}

void sign_band(int sign_time, Band *band, State *state, int distribution, float n_percentage, float u_percentage, float r_percentage, int schedulerType){
    while(TRUE){
        int left_array_index = 0;  // Save the current position into left array
        int right_array_index = 0; // Save the current position into right array
        Array *left_array = package_gen(distribution, n_percentage, u_percentage, r_percentage);
        Array *right_array = package_gen(distribution, n_percentage, u_percentage, r_percentage);
        print_packages(left_array); //TODO: Lineas de prueba, eliminar
        print_packages(right_array); //TODO: Lineas de prueba, eliminar
        int left_index_counter[left_array->length];
        int right_index_counter[right_array->length];
        int sign_time_counter = 0;
        int transport_time = 0;
        State prev_state;
        Package *package;
        while(left_array->length != left_array_index || right_array->length != right_array_index){

            if(band->sign_side == LEFT){ // If the current side is left, we use a package of the left
                if(left_array->length != left_array_index){
                    // if(schedulerType == 1){
                    //     left_array_index = priority_scheduler(left_array,left_index_counter,left_array->length);
                    //     left_index_counter[left_array_index] = left_array_index;
                    //     printf("%d \n",left_index_counter[left_array_index]);
                    // }
                    // else if (schedulerType == 2){
                    //     left_array_index = shorter_scheduler(left_array,left_index_counter, left_array->length);
                    //     left_index_counter[left_array_index] = left_array_index;
                    // }
                    package = &left_array->package_array[left_array_index]; // Define a package to extract the information
                    left_array_index += 1; // Increment the left index
                    transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                    state->package = *package;  // Defining which package is used at the time
                    state->percentage = 0;
                } else{
                    package = &right_array->package_array[right_array_index]; // Define a package to extract the information
                    right_array_index += 1; // Increment the left index
                    transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                    state->package = *package; // Defining which package is used at the time
                    state->percentage = 0;
                }
            } else { // If the current side is right, we use a package of the right
                if(right_array->length != right_array_index){
                    package = &right_array->package_array[right_array_index]; // Define a package to extract the information
                    right_array_index += 1; // Increment the left index
                    transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                    state->package = *package; // Defining which package is used at the time
                    state->percentage = 0;
                } else {
                    package = &left_array->package_array[left_array_index]; // Define a package to extract the information
                    left_array_index += 1; // Increment the left index
                    transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                    state->package = *package;  // Defining which package is used at the time
                    state->percentage = 0;
                }
            }
            for(int i = 0; i < transport_time; i++){ // Loop to simulate the package transport
                state->state = i;                    // Save the current loop's current state
                state->percentage = (int)(((float)(i+1)/(float)transport_time)*100);
                sleep(1);                    // Wait for a second
                sign_time_counter += 1;
                if(sign_time_counter == sign_time){
                    band->sign_side = !band->sign_side;
                    sign_time_counter = 0;
                    if(band->sign_side == LEFT && left_array->length != left_array_index && state->package.type != RADIOACTIVE){
                        if(left_array->package_array[left_array_index].type == RADIOACTIVE){
                            prev_state.package = state->package;
                            prev_state.state = prev_state.state;
                            package = &left_array->package_array[left_array_index];
                            left_array_index += 1; // Increment the left index
                            transport_time = get_t_time(package->weight, band->band_length, band->band_strength);
                            state->package = *package;  // Defining which package is used at the time
                            state->percentage = 0;
                            for(int k = 0; k<transport_time; k++){
                                state->state = k;                    // Save the current loop's current state
                                state->percentage = (int)(((float)(k+1)/(float)transport_time)*100);
                                sleep(1);                    // Wait for a second
                            }
                            printf("%d \n", band->sign_side); //TODO: lineas de prueba, eliminar
                            state->state = prev_state.state;
                            state->package = prev_state.package;
                            package = &state->package;
                            transport_time = get_t_time(package->weight, band->band_length, band->band_strength);
                            state->percentage = (int)ceil((i/transport_time)*100);
                            continue;
                        } else {
                            continue;
                        }
                    }
                    if(band->sign_side == RIGHT && right_array->length != right_array_index && state->package.type != RADIOACTIVE){
                        if(right_array->package_array[right_array_index].type == RADIOACTIVE){
                            prev_state.package = state->package;
                            prev_state.state = prev_state.state;
                            package = &right_array->package_array[right_array_index];
                            right_array_index += 1; // Increment the left index
                            transport_time = get_t_time(package->weight, band->band_length, band->band_strength);
                            state->package = *package;  // Defining which package is used at the time
                            state->percentage = 0;
                            for(int k = 0; k<transport_time; k++){
                                state->state = k;                    // Save the current loop's current state
                                state->percentage = (int)(((float)(k+1)/(float)transport_time)*100);
                                sleep(1);                    // Wait for a second
                            }
                            printf("%d \n", band->sign_side); //TODO: lineas de prueba, eliminar
                            state->state = prev_state.state;
                            state->package = prev_state.package;
                            package = &state->package;
                            transport_time = get_t_time(package->weight, band->band_length, band->band_strength);
                            state->percentage = (int)(((float)(i+1)/(float)transport_time)*100);
                            continue;
                        }else{
                            continue;
                        }
                    } else{
                        continue;
                    }

                } else {
                    continue;
                }
            }
            printf("%d \n", band->sign_side); //TODO: lineas de prueba, eliminar
        }
    }

}


void random_band(Band *band, State *state, int distribution, float n_percentage, float u_percentage, float r_percentage){
    while(TRUE){
        int left_array_index = 0;
        int right_array_index = 0;
        Array *left_array = package_gen(distribution, n_percentage, u_percentage, r_percentage);
        Array *right_array = package_gen(distribution, n_percentage, u_percentage, r_percentage);
        print_packages(left_array); //TODO: Lineas de prueba, eliminar
        print_packages(right_array); //TODO: Lineas de prueba, eliminar
        band->sign_side = genRandoms(0,1);
        int transport_time;
        Package *package;
        // Validating that the index are still into the arrays
        while(left_array->length != left_array_index || right_array->length != right_array_index){
            if(band->sign_side == LEFT){ // If the current side is left, we use a package of the left
                if(left_array->length != left_array_index){
                    package = &left_array->package_array[left_array_index]; // Define a package to extract the information
                    left_array_index += 1; // Increment the left index
                    transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                    state->package = *package;  // Defining which package is used at the time
                    state->percentage = 0;
                } else{
                    package = &right_array->package_array[right_array_index]; // Define a package to extract the information
                    right_array_index += 1; // Increment the left index
                    transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                    state->package = *package; // Defining which package is used at the time
                    state->percentage = 0;
                }
            } else { // If the current side is right, we use a package of the right
                if(right_array->length != right_array_index){
                    package = &right_array->package_array[right_array_index]; // Define a package to extract the information
                    right_array_index += 1; // Increment the left index
                    transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                    state->package = *package; // Defining which package is used at the time
                    state->percentage = 0;
                } else {
                    package = &left_array->package_array[left_array_index]; // Define a package to extract the information
                    left_array_index += 1; // Increment the left index
                    transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                    state->package = *package;  // Defining which package is used at the time
                    state->percentage = 0;
                }
            }
            for(int i = 0; i < transport_time; i++){
                state->state = i;                    // Save the current loop's current state
                state->percentage = (int)(((float)(i + 1) / (float)transport_time) * 100);
                sleep(1);                    // Wait for a second
            }
            printf("%d \n", band->sign_side); //TODO: lineas de prueba, eliminar
            band->sign_side = genRandoms(0,1);
        }
    }
}

/*
void create_bands(){ //TODO: Implementar archivo de configuración acá
    if(algorithm_mode == 0){
        package_equity_band(W, band, left_array, right_array, state);
    }
    else if(algorithm_mode == 1){
        sign_band(sign_time, band, left_array, right_array, state);
    } else{
        random_band(band, left_array, right_array, state);
    }
}
*/

int main(){
    // to the random
    time_t t;
    srand((unsigned) time(&t));
    //
    int w = 3;
    int sign_time = 3;
    Band band;
    band.band_strength = 100;
    band.band_length = 100;
    State state;
    package_equity_band(w, &band, &state, 0, 0.1f, 0.4f, 0.5f,2);
    //sign_band(sign_time, &band, &state, 0, 0.1f, 0.4f, 0.5f,4);
    //random_band(&band, &state, 0, 0.1f, 0.4f, 0.5f);
    return 0;
}
