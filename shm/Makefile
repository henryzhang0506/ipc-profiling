ALL: pub sub

.PHONY:pub
pub:
	g++ -O3 -g3 -std=c++14 -o pub pub.cpp -lrt

.PHONY:sub
sub:
	g++ -O3 -g3 -std=c++14 -o sub sub.cpp -lrt

.PHONY:clean
clean:
	rm -rf pub sub
