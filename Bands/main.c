#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <time.h>
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
    State state;
    int algorithm_mode = 0;
    int W = 2;
    int sign_time = 5;
    Band band;
    band.band_length = 2;
    band.band_strength = 50;
    Array left_array;
    left_array.length = 4;
    left_array.package_array = malloc(left_array.length* sizeof(Package));
    if(left_array.package_array == NULL){
        printf("Memory hasn't been allocated correctly \n");
        exit(0);
    }
    Array right_array;
    right_array.length = 3;
    right_array.package_array = malloc(right_array.length* sizeof(Package));
    if(right_array.package_array == NULL){
        printf("Memory hasn't been allocated correctly \n");
        exit(0);
    }
    Package pack;
    pack.weight = 2;
    pack.type = 0;

    for(int i = 0; i < left_array.length; i++) {
        left_array.package_array[i] = pack;
        pack.weight = pack.weight + 1;
        //printf("%d: %d \n", i, left_array.package_array[i].weight);
    }
    pack.weight = 2;
    for(int i = 0; i < right_array.length; i++){
        right_array.package_array[i] = pack;
        pack.weight = pack.weight + 1;
        //printf("%d: %d \n", i, right_array.package_array[i].weight);
    }
    //create_band(algorithm_mode, W, sign_time, &band, &left_array, &right_array, &state);
    create_band(2, 0, 0, &band, &left_array, &right_array, &state);
    return 0;
}