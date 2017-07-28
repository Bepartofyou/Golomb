# Brief
* Golomb-Rice and Exponental-Golomb aigorithm implementation

## Build
* mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Debug ../ && make

## Usage
* ./test -h

      Golomb 0.0.0

      Golomb algorithm learning and implementation.

      By Bepartofyou <309554135@qq.com>

      Usage: Golomb [OPTIONS]...

        -h, --help           Print help and exit
        -V, --version        Print version and exit
        -a, --algorithm=INT  the Golomb coding algorithm  (default=`1')
        -p, --parameter=INT  the tunable parameter to divide an input value for 
                               Golomb coding, when  a=1, it means Golomb-Rice 
                               parameter; when a=2, it means the level of 
                               Exponential-Golomb
        -N, --encodenum=INT  the input value to Golomb coding
  
* ./test  -a 1 -p 8 -N 14


      Golomb-Rice encode bits of 14 :
                                   10011
      Golomb-Rice decode bits of  10011  :
                                    14
                                    
* ./test  -a 2 -p 8 -N 14 

      Exponential-Golomb encode bits of 14 :
                                           00000000101110000
      Exponential-Golomb decode bits of  00000000101110000 :
                                           14

## Refs
* https://github.com/brookicv/GolombCode
* http://blog.csdn.net/brookicv/article/details/54603650
