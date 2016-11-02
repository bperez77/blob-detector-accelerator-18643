//this is just a simple summing kernel
OUT_T summing_kernal(IN_T **window, int width, int height){
    IN_T sum;
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            sum += window[i][j]; 
        }
    } 
    return (OUT_T)sum;
}

