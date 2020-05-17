// VolEsti (volume computation and sampling library)

// Copyright (c) 2012-2020 Vissarion Fisikopoulos
// Copyright (c) 2018-2020 Apostolos Chalkis

// Licensed under GNU LGPL.3, see LICENCE file

#ifndef RANDOM_WALKS_BOUNDARY_RDHR_WALK_HPP
#define RANDOM_WALKS_BOUNDARY_RDHR_WALK_HPP

#include "convex_bodies/ball.h"
#include "convex_bodies/ballintersectconvex.h"
#include "sampling/sphere.hpp"

// Random directions hit-and-run walk with uniform target distribution

struct BRDHRWalk
{

    template
            <
                    typename Polytope,
                    typename RandomNumberGenerator
            >
    struct Walk
    {
        typedef typename Polytope::PointType Point;
        typedef typename Point::FT NT;
        typedef Ball<Point> BallType;
        typedef BallIntersectPolytope<Polytope,BallType> BallPolytope;

        Walk(Polytope const& P, Point & p, RandomNumberGenerator &rng)
        {
            initialize(P, p, rng);
        }

        Walk(BallPolytope const& P, Point & p, RandomNumberGenerator &rng)
        {
            initialize(P, p, rng);
        }
        Walk (BallType const&, Point &, RandomNumberGenerator &) {}

        template
                <
                        typename BallPolytope
                >
        inline void apply(BallPolytope const& P,
                          Point &p1,   // a point to start
                          Point &p2,
                          unsigned int const& walk_length,
                          RandomNumberGenerator &rng)
        {
            for (auto j=0u; j<walk_length; ++j)
            {
                Point v = GetDirection<Point>::apply(p1.dimension(), rng);
                std::pair<NT, NT> bpair = P.line_intersect(_p, v, _lamdas, _Av,
                                                           _lambda);
                _lambda = rng.sample_urdist() * (bpair.first - bpair.second)
                          + bpair.second;
                p1 = _p + bpair.first * v;
                p2 = _p + bpair.second * v;
                _p += (_lambda * v);
            }
        }

    private :

        template <typename GenericBody>
        inline void initialize(GenericBody const& P,
                               Point &p,
                               RandomNumberGenerator &rng)
        {
            _lamdas.setZero(P.num_of_hyperplanes());
            _Av.setZero(P.num_of_hyperplanes());

            Point v = GetDirection<Point>::apply(p.dimension(), rng);
            std::pair<NT, NT> bpair = P.line_intersect(p, v, _lamdas, _Av);
            _lambda = rng.sample_urdist() * (bpair.first - bpair.second) + bpair.second;
            _p = (_lambda * v) + p;
        }

        Point _p;
        NT _lambda;
        typename Point::Coeff _lamdas;
        typename Point::Coeff _Av;
    };

};


#endif // RANDOM_WALKS_BOUNDARY_RDHR_WALK_HPP
