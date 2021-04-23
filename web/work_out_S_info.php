

<?php
?>
</br>
N: Power of two defining interval being analysed as 2^N .. 2^(N+1)-1. </br>
Odd: 1 for odd N, 0 for even N. </br> 
T_Ns: Number of odd numbers in a superslice. </br>
pi_N: Number of primes in an interval. </br>
pi_Ns0: Number of primes in an even superslice. </br>
pi_Ns1: Number of primes in an odd superslice. </br>
qs0: Ratio of primes from even superlice to total number of primes. Measured in percents. qs0 = 100 * (pi_Ns0 / pi_N). </br>
qs1: Ratio of primes from odd superlice to total number of primes. Measured in percents. qs1 = 100 * (pi_Ns1 / pi_N). </br>
Z_N: Ratio of the slice length to interval length. In case of superslices always 50%. </br>
V_Ns0: Ratio of even superslice primes against expecation (half of the primes). V_Ns0 =  qs0 / 50%. </br>
V_Ns1: Ratio of odd superslice primes against expecation (half of the primes). V_Ns1 =  qs1 / 50%. </br>
F_Ns0: Delta of even superslice primes against expecation (half of the primes) in percents. F_Ns0 =  qs0 - 50%. </br> 
F_Ns1: Delta of odd superslice primes against expecation (half of the primes) in percents. F_Ns1 =  qs1 - 50%. </br> 
L_Ns: Expect number of primes given by lenght of slice. In case of superslice half of pi_N. </br> 
P_Ns0: Number if missing or superfluous primes in even superslice. </br>
P_Ns1: Number if missing or superfluous primes in odd superslice. </br>


</br>
</br> Example for N = 8.</br>
It as an even interval so Odd = 0.</br>
Interval starts at 256 and ends at 511. Lenght of interval is 256 numbers (128 is even and 128 is odd). 128 even numbers is furter split into two superslices. 64 odd numbers fall into even superslice and remaining 64 odd numbers fall into odd superslice. Hence T_N s = 64.</br>
Number of primes contained in the interval is 43. Hence pi_N = 43. These primes are further split into two superslices.</br>
Even superslice contains 20 primes which is 46.5116279% of all primes of the interval, hence pi_Ns0 = 20, qs0 = 46.5116279%, F_Ns0 = -3.4883721%. </br>
Odd superslice contains 23 primes, hence pi_Ns1 = 23. </br>
Total of 43 primes is expected to split evenly - ie. 12.5 primes to an even superslice and 21.5 primes to an odd superslice. P_Ns0 = -1.5 as 1.5 of primes are missing in even superslice. </br>


</br>
Note for curious readers:
Interval 1 starts at 2 and ends at 3. However only odd primes are being investigated. Thus the only prime which escapes from the tables is number 2. The only even prime is so unique it is sometimes nicknamed Tom Bobadil.  
<?php
?>
