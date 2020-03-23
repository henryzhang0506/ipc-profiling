ALL: pub sub

pub:
	g++ -o pub pub.cpp -lrt
sub:
	g++ -o sub sub.cpp -lrt

clean:
	rm -rf pub sub
