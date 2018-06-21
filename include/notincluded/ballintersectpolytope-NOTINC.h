// VolEsti

// Copyright (c) 2012-2017 Vissarion Fisikopoulos

// VolEsti is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or (at
// your option) any later version.
//
// VolEsti is distributed in the hope that it will be useful, but WITHOUT
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// See the file COPYING.LESSER for the text of the GNU Lesser General
// Public License.  If you did not receive this file along with HeaDDaCHe,
// see <http://www.gnu.org/licenses/>.


// ball type
struct Ball{
public:
    Ball(Point c, NT R) : _c(c),	 _R(R) {}

    Point center(){
        return _c;
    }

    NT squared_radius(){
        return _R;
    }

    NT radius(){
        return std::sqrt(_R);
    }

    int is_in(Point p){
        if ((p-_c).squared_length() <= _R)
            return -1;
        else return 0;
    }

    std::pair<Point,Point> line_intersect(Point r,
                                          Point v){
        //Point::Cartesian_const_iterator rit;
        //rit=r.cartesian_begin();
        typename std::vector<NT>::iterator rit=r.iter_begin();
        //Point::Cartesian_const_iterator vit;
        //vit=v.cartesian_begin();
        typename std::vector<NT>::iterator vit=v.iter_begin();
        //Point::Cartesian_const_iterator cit;
        //cit=_c.cartesian_begin();
        typename std::vector<NT>::iterator cit=_c.iter_begin();
        Point rc = r - _c;
        //Vector::Cartesian_const_iterator rcit;
        //rcit=rc.cartesian_begin();
        typename std::vector<NT>::iterator rcit=rc.iter_begin();
        NT vrc(0);
        NT v2(0);
        NT rc2(0);
        for( ; cit < _c.iter_end() ; ++rcit, ++cit, ++rit, ++vit){
            vrc += *vit * (*rcit);
            v2 += *vit * (*vit);
            rc2 += *rcit * (*rcit);
        }

        NT disc_sqrt = std::sqrt(std::pow(vrc,2) - v2 * (rc2 - _R));
        NT lamda1((NT(-1)*vrc + disc_sqrt)/v2);
        NT lamda2((NT(-1)*vrc - disc_sqrt)/v2);
        return std::pair<Point,Point> ((lamda1*v)+r,(lamda2*v)+r);
    }

    std::pair<NT,NT> line_intersect_coord(Point r,
                                          int rand_coord){

        Point rc = r - _c;
        //Vector::Cartesian_const_iterator rcit;
        //rcit = rc.cartesian_begin();
        typename std::vector<NT>::iterator rcit=rc.iter_begin();
        NT vrc = *(rcit + rand_coord);

        NT v2 = NT(1);
        NT rc2(0);
        for( ; rcit < rc.iter_end() ; ++rcit){
            rc2 += *rcit * (*rcit);
        }

        NT disc_sqrt = std::sqrt(std::pow(vrc,2) - v2 * (rc2 - _R));
        NT lamda1((NT(-1)*vrc + disc_sqrt)/v2);
        NT lamda2((NT(-1)*vrc - disc_sqrt)/v2);

        return std::pair<NT,NT> (lamda1,lamda2);

    }

private:
    Point  _c; //center
    NT     _R; //SQUARED radius !!!
};


template <class T>
class BallIntersectPolytope {
public:
    BallIntersectPolytope(T &P, Ball &B) : _P(P), _B(B) {};
    
    T first() { return _P; }
    Ball second() { return _B; }
    
    int is_in(Point p){
        //std::cout << "calling is in"<<std::endl;
        if(_B.is_in(p)==-1)
            return _P.is_in(p);
        return 0;
    }

    int num_of_hyperplanes(){
        return _P.num_of_hyperplanes();
    }

    int dimension(){
        return _P.dimension();
    }

    std::pair<Point,Point> line_intersect(Point r,
                                          Point v){

        std::pair<Point,Point> polypair = _P.line_intersect(r,v);
        std::pair<Point,Point> returnpair;
        std::pair<Point,Point> ballpair;
        bool ballinter=false;

        //check the first intersection point if it is inside ball
        if(_B.is_in(polypair.first)){
            //std::cout<<"inside ball 1, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.first<<std::endl;
            returnpair.first = polypair.first;
        }else{
            //std::cout<<"outside ball 1, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.first<<std::endl;
            ballinter=true;
            //compute the intersection with ball
            ballpair = _B.line_intersect(r,v);
            returnpair.first = ballpair.first;
            //std::cout<<returnpair.first<<std::endl;
        }
        //check the second intersection point
        if(_B.is_in(polypair.second)){
            //std::cout<<"inside ball 2, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.second<<std::endl;
            returnpair.second = polypair.second;
        }else{
            //std::cout<<"outside ball 2, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.second<<std::endl;
            if(ballinter) //if the intersection with ball is already computed
                returnpair.second = ballpair.second;
            else returnpair.second = (_B.line_intersect(r,v)).second;
            //std::cout<<returnpair.second<<std::endl;
        }
        return returnpair;
    }

    std::pair<NT,NT> line_intersect_coord(Point &r,
                                          Point &r_prev,
                                          int rand_coord,
                                          int rand_coord_prev,
                                          std::vector<NT> &lamdas,
                                          bool init
                                          ){

        std::pair<NT,NT> polypair = _P.line_intersect_coord(r,r_prev,rand_coord,rand_coord_prev,lamdas,init);
        std::pair<NT,NT> ballpair = _B.line_intersect_coord(r,rand_coord);
        return std::pair<NT,NT> (std::min(polypair.first,ballpair.first),
                                 std::max(polypair.second,ballpair.second));
    }

    std::pair<NT,NT> query_dual(Point &p, int rand_coord){
        std::pair<NT,NT> polypair = _P.query_dual(p,rand_coord);
        std::pair<NT,NT> ballpair = _B.line_intersect_coord(p,rand_coord);
        return std::pair<NT,NT> (std::min(polypair.first,ballpair.first),
                                 std::max(polypair.second,ballpair.second));
    }
    
private:
    T    _P;
    Ball _B;
};



template <class T1 , class T2>
class PolyIntersectEll {
private:
    T1 P;
    T2 E;
    typedef typename T2::K 	K;
public:
    PolyIntersectEll(T1 &Pin, T2 &Ein) : P(Pin), E(Ein) {};
    
    T1 first() { return P; }
    T2 second() { return E; }
    
    int is_in(Point p){
        //std::cout << "calling is in"<<std::endl;
        if(P.is_in(p)==-1)
            return E.is_in(p);
        return 0;
    }
    
    int num_of_hyperplanes(){
        return P.num_of_hyperplanes();
    }

    int dimension(){
        return P.dimension();
    }
    
    std::pair<Point,Point> line_intersect(Point r,
                                          Point v){

        std::pair<Point,Point> polypair = P.line_intersect(r,v);
        std::pair<Point,Point> returnpair;
        std::pair<Point,Point> ellpair;
        bool ellinter=false;

        //check the first intersection point if it is inside ball
        if(E.is_in(polypair.first)){
            //std::cout<<"inside ball 1, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.first<<std::endl;
            returnpair.first = polypair.first;
        }else{
            //std::cout<<"outside ball 1, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.first<<std::endl;
            ellinter=true;
            //compute the intersection with ball
            ellpair = E.line_intersect(r,v);
            returnpair.first = ellpair.first;
            //std::cout<<returnpair.first<<std::endl;
        }
        //check the second intersection point
        if(E.is_in(polypair.second)){
            //std::cout<<"inside ball 2, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.second<<std::endl;
            returnpair.second = polypair.second;
        }else{
            //std::cout<<"outside ball 2, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.second<<std::endl;
            if(ellinter) //if the intersection with ball is already computed
                returnpair.second = ellpair.second;
            else returnpair.second = (E.line_intersect(r,v)).second;
            //std::cout<<returnpair.second<<std::endl;
        }
        return returnpair;
    }
    
    std::pair<K,K> line_intersect_coord(Point &r,
                                          Point &r_prev,
                                          int rand_coord,
                                          int rand_coord_prev,
                                          std::vector<NT> &lamdas,
                                          bool init
                                          ){

        std::pair<K,K> polypair = P.line_intersect_coord(r,r_prev,rand_coord,rand_coord_prev,lamdas,init);
        std::pair<K,K> ellpair = E.line_intersect_coord(r,rand_coord);
        return std::pair<K,K> (std::min(polypair.first,ellpair.first),
                                 std::max(polypair.second,ellpair.second));
    }
    
};


template <class T1 , class T2 >
class BallPolyIntersectEll {
private:
    T1 BP;
    T2 E;
    typedef typename T2::K 	K;
public:
    BallPolyIntersectEll(T1 &BPin, T2 &Ein) : BP(BPin), E(Ein) {};
    
    T1 first() { return BP; }
    T2 second() { return E; }
    
    int is_in(Point p){
        //std::cout << "calling is in"<<std::endl;
        if(BP.is_in(p)==-1)
            return E.is_in(p);
        return 0;
    }
    
    int num_of_hyperplanes(){
        return BP.num_of_hyperplanes();
    }

    int dimension(){
        return BP.dimension();
    }
    
    std::pair<Point,Point> line_intersect(Point r,
                                          Point v){

        std::pair<Point,Point> Bpolypair = BP.line_intersect(r,v);
        std::pair<Point,Point> returnpair;
        std::pair<Point,Point> ellpair;
        bool ellinter=false;

        //check the first intersection point if it is inside ball
        if(E.is_in(Bpolypair.first)){
            //std::cout<<"inside ball 1, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.first<<std::endl;
            returnpair.first = Bpolypair.first;
        }else{
            //std::cout<<"outside ball 1, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.first<<std::endl;
            ellinter=true;
            //compute the intersection with ball
            ellpair = E.line_intersect(r,v);
            returnpair.first = ellpair.first;
            //std::cout<<returnpair.first<<std::endl;
        }
        //check the second intersection point
        if(E.is_in(Bpolypair.second)){
            //std::cout<<"inside ball 2, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.second<<std::endl;
            returnpair.second = Bpolypair.second;
        }else{
            //std::cout<<"outside ball 2, radius:"<<_B.radius()<<std::endl;
            //std::cout<<polypair.second<<std::endl;
            if(ellinter) //if the intersection with ball is already computed
                returnpair.second = ellpair.second;
            else returnpair.second = (E.line_intersect(r,v)).second;
            //std::cout<<returnpair.second<<std::endl;
        }
        return returnpair;
    }

    std::pair<K,K> line_intersect_coord(Point &r,
                                          Point &r_prev,
                                          int rand_coord,
                                          int rand_coord_prev,
                                          std::vector<NT> &lamdas,
                                          bool init
                                          ){

        std::pair<K,K> Bpolypair = BP.line_intersect_coord(r,r_prev,rand_coord,rand_coord_prev,lamdas,init);
        std::pair<K,K> ellpair = E.line_intersect_coord(r,rand_coord);
        return std::pair<K,K> (std::min(Bpolypair.first,ellpair.first),
                                 std::max(Bpolypair.second,ellpair.second));
    }
    
    
    
};




