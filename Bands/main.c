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

typedef struct Package{
    int weight;          // Band's weight
    int type;            // Band's type
} Package;

typedef struct State{
    int state;
    Package package;
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
    time_t t;
    srand((unsigned) time(&t));
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
    time_t t;
    srand((unsigned) time(&t));
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
    if(distribution == 0){ //Normal
        package_num = normalRandom();
    } else { //poisson
        package_num = poisson();
    }
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
            package.weight = genRandoms(0,20);
            packages_list->package_array[counter] = package;
            radioactive_num -= 1;
            counter += 1;
            continue;
        } else if ((t == URGENT && urgent_num != 0) || ((radioactive_num == 0 || normal_num == 0) && urgent_num != 0)){
            Package package;
            package.type = URGENT;
            package.weight = genRandoms(0,20);
            packages_list->package_array[counter] = package;
            urgent_num -= 1;
            counter += 1;
            continue;
        } else if((t == NORMAL && normal_num != 0) || ((radioactive_num == 0 || urgent_num == 0) && normal_num != 0)){
            Package package;
            package.type = NORMAL;
            package.weight = genRandoms(0,20);
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

void package_equity_band(int W, Band *band, Array *left_array, Array *right_array, State *state){
    int package_counter = 0;   // Save the number of packages that have been passed through the band
    int left_array_index = 0;  // Save the current position into left array
    int right_array_index = 0; // Save the current position into right array
    int current_side = LEFT;
    int transport_time;
    // Validating that the index are still into the arrays
    while(left_array->length != left_array_index || right_array->length != right_array_index){

        if(current_side == LEFT){ // If the current side is left, we use a package of the left
            Package *package = &left_array->package_array[left_array_index]; // Define a package to extract the information
            left_array_index += 1; // Increment the left index
            transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
            state->package = *package;  // Defining which package is used at the time
        }
        if(current_side == RIGHT){ // If the current side is right, we use a package of the right
            Package *package = &right_array->package_array[right_array_index]; // Define a package to extract the information
            right_array_index += 1; // Increment the left index
            transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
            state->package = *package; // Defining which package is used at the time
        }
        for(int i = 0; i < transport_time; i++){ // Loop to simulate the package transport
            state->state = i;                    // Save the current loop's current state
            sleep(1);                    // Wait for a second
        }
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

void sign_band(int sign_time, Band *band, Array *left_array, Array *right_array, State *state){
    int left_array_index = 0;  // Save the current position into left array
    int right_array_index = 0; // Save the current position into right array
    int sign_time_counter = 0;
    int transport_time = 0;
    State prev_state;
    Package *package;
    while(left_array->length != left_array_index || right_array->length != right_array_index){

        if(band->sign_side == LEFT){ // If the current side is left, we use a package of the left
            if(left_array->length != left_array_index){
                package = &left_array->package_array[left_array_index]; // Define a package to extract the information
                left_array_index += 1; // Increment the left index
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                state->package = *package;  // Defining which package is used at the time
            } else{
                package = &right_array->package_array[right_array_index]; // Define a package to extract the information
                right_array_index += 1; // Increment the left index
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                state->package = *package; // Defining which package is used at the time
            }
        }
        if(band->sign_side == RIGHT){ // If the current side is right, we use a package of the right
            if(right_array->length != right_array_index){
                package = &right_array->package_array[right_array_index]; // Define a package to extract the information
                right_array_index += 1; // Increment the left index
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                state->package = *package; // Defining which package is used at the time
            } else {
                package = &left_array->package_array[left_array_index]; // Define a package to extract the information
                left_array_index += 1; // Increment the left index
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                state->package = *package;  // Defining which package is used at the time
            }
        }
        for(int i = 0; i < transport_time; i++){ // Loop to simulate the package transport
            state->state = i;                    // Save the current loop's current state
            sleep(1);                    // Wait for a second
            sign_time_counter += 1;
            if(sign_time_counter == sign_time){
                band->sign_side = !band->sign_side;
                sign_time_counter = 0;
                if(band->sign_side == LEFT && left_array->length != left_array_index){
                    if(left_array->package_array[left_array_index].type == RADIOACTIVE){
                        prev_state.package = state->package;
                        prev_state.state = prev_state.state;
                        package = &left_array->package_array[left_array_index];
                        left_array_index += 1; // Increment the left index
                        transport_time = get_t_time(package->weight, band->band_length, band->band_strength);
                        state->package = *package;  // Defining which package is used at the time
                        for(int k = 0; k<transport_time; k++){
                            state->state = k;                    // Save the current loop's current state
                            sleep(1);                    // Wait for a second
                            sign_time_counter += 1;
                        }
                        state->state = prev_state.state;
                        state->package = prev_state.package;
                        package = &state->package;
                        transport_time = get_t_time(package->weight, band->band_length, band->band_strength);
                        continue;
                    } else {
                        continue;
                    }
                }
                if(band->sign_side == RIGHT && right_array->length != right_array_index){
                    if(right_array->package_array[right_array_index].type == RADIOACTIVE){
                        prev_state.package = state->package;
                        prev_state.state = prev_state.state;
                        package = &right_array->package_array[right_array_index];
                        right_array_index += 1; // Increment the left index
                        transport_time = get_t_time(package->weight, band->band_length, band->band_strength);
                        state->package = *package;  // Defining which package is used at the time
                        for(int k = 0; k<transport_time; k++){
                            state->state = k;                    // Save the current loop's current state
                            sleep(1);                    // Wait for a second
                            sign_time_counter += 1;
                        }
                        state->state = prev_state.state;
                        state->package = prev_state.package;
                        package = &state->package;
                        transport_time = get_t_time(package->weight, band->band_length, band->band_strength);
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
    }
}


void random_band(Band *band, Array *left_array, Array *right_array, State *state){
    int left_array_index = 0;
    int right_array_index = 0;
    int current_side = genRandoms(0,1);
    int transport_time;
    int change_side = TRUE;
    // Validating that the index are still into the arrays
    while(left_array->length != left_array_index || right_array->length != right_array_index){
        if(change_side == TRUE){
            if(current_side == LEFT){ // If the current side is left, we use a package of the left
                Package *package = &left_array->package_array[left_array_index]; // Define a package to extract the information
                left_array_index += 1; // Increment the left index
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                state->package = *package;  // Defining which package is used at the time
            }
            if(current_side == RIGHT){ // If the current side is right, we use a package of the right
                Package *package = &right_array->package_array[right_array_index]; // Define a package to extract the information
                right_array_index += 1; // Increment the left index
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                state->package = *package; // Defining which package is used at the time
            }
            for(int i = 0; i < transport_time; i++){ // Loop to simulate the package transport
                state->state = i;                    // Save the current loop's current state
                sleep(1);                    // Wait for a second
            }
            if(current_side == LEFT && right_array_index == right_array->length){ // If the maximum array value was reached, change side
                change_side = FALSE;
                continue;
            } else if(current_side == RIGHT && left_array_index == left_array->length){ // If the maximum array value was reached, change side
                change_side = FALSE;
                continue;
            } else {
                current_side = genRandoms(0,1);
                continue;
            }
        } else {
            if(current_side == LEFT){ // If the current side is left, we use a package of the left
                Package *package = &left_array->package_array[left_array_index]; // Define a package to extract the information
                left_array_index += 1; // Increment the left index
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                state->package = *package;  // Defining which package is used at the time
            }
            if(current_side == RIGHT){ // If the current side is right, we use a package of the right
                Package *package = &right_array->package_array[right_array_index]; // Define a package to extract the information
                right_array_index += 1; // Increment the left index
                transport_time = get_t_time(package->weight, band->band_length, band->band_strength); /*Getting the time that the
            * package will take to pass the through the band*/
                state->package = *package; // Defining which package is used at the time
            }
            for(int i = 0; i < transport_time; i++){ // Loop to simulate the package transport
                state->state = i;                    // Save the current loop's current state
                sleep(1);                    // Wait for a second
            }
            continue;
        }
    }
}


void create_band(int algorithm_mode, int W, int sign_time, Band *band, Array *left_array, Array *right_array, State *state){
    if(algorithm_mode == 0){
        package_equity_band(W, band, left_array, right_array, state);
    }
    else if(algorithm_mode == 1){
        sign_band(sign_time, band, left_array, right_array, state);
    } else{
        random_band(band, left_array, right_array, state);
    }
}


int main(){
    Array* package_list = package_gen(0, 0.10, 0.5, 0.4);
    int r = 0;
    int u = 0;
    int n = 0;
    for(int i = 0; i < package_list->length; i++){
        int k = package_list->package_array[i].type;
        if(k == RADIOACTIVE){
            printf("Radioactivo \n");
            r += 1;
        } else if(k == URGENT){
            printf("Urgente \n");
            u += 1;
        } else {
            printf("Normal \n");
            n += 1;
        }
    }
    printf("Radioactivos: %d \nUrgentes: %d \nNormales: %d \nTotal: %d \n", r, u, n, package_list->length);
    return 0;
}