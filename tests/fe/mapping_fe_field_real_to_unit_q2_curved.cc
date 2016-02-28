// ---------------------------------------------------------------------
//
// Copyright (C) 2006 - 2015 by the deal.II authors
//
// This file is part of the deal.II library.
//
// The deal.II library is free software; you can use it, redistribute
// it, and/or modify it under the terms of the GNU Lesser General
// Public License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
// The full text of the license can be found in the file LICENSE at
// the top level of the deal.II distribution.
//
// ---------------------------------------------------------------------



// on a somewhat deformed cube, verify that if we push forward a bunch
// of points from the reference to the real cell and then call
// MappingFEField::transform_unit_to_real_cell that we get the same point as
// we had in the beginning.

// We use a Q5 mapping but this time we
// actually curve one boundary of the cell which ensures that the
// mapping is really higher order than just Q1

#include "../tests.h"

#include <deal.II/base/utilities.h>
#include <deal.II/grid/tria.h>
#include <deal.II/grid/grid_generator.h>
#include <deal.II/fe/mapping_fe_field.h>
#include <deal.II/fe/fe_q.h>
#include <deal.II/fe/component_mask.h>
#include <deal.II/fe/mapping_q1.h>
#include <deal.II/fe/mapping_q.h>
#include <deal.II/fe/fe_system.h>
#include <deal.II/grid/tria_boundary_lib.h>
#include <deal.II/numerics/vector_tools.h>

#include <deal.II/grid/grid_out.h>
#include <fstream>

using namespace dealii;


template<int dim, int spacedim>
void test_real_to_unit_cell()
{
  deallog << "dim=" << dim << ", spacedim=" << spacedim << std::endl;

  // define a boundary that fits the
  // the vertices of the hyper cube
  // we're going to create below
  HyperBallBoundary<dim,spacedim> boundary (Point<spacedim>(),
                                            std::sqrt(1.*dim));

  Triangulation<dim, spacedim>   triangulation;
  GridGenerator::hyper_cube (triangulation, -1, 1);

  // set the boundary indicator for
  // one face of the single cell
  triangulation.set_boundary (1, boundary);
  triangulation.begin_active()->face(0)->set_boundary_id (1);


  const unsigned int n_points = 5;
  std::vector< Point<dim> > unit_points(Utilities::fixed_power<dim>(n_points));

  switch (dim)
    {
    case 1:
      for (unsigned int x=0; x<n_points; ++x)
        unit_points[x][0] = double(x)/double(n_points);
      break;

    case 2:
      for (unsigned int x=0; x<n_points; ++x)
        for (unsigned int y=0; y<n_points; ++y)
          {
            unit_points[y * n_points + x][0] = double(x)/double(n_points);
            unit_points[y * n_points + x][1] = double(y)/double(n_points);
          }
      break;

    case 3:
      for (unsigned int x=0; x<n_points; ++x)
        for (unsigned int y=0; y<n_points; ++y)
          for (unsigned int z=0; z<n_points; ++z)
            {
              unit_points[z * n_points * n_points + y * n_points + x][0] = double(x)/double(n_points);
              unit_points[z * n_points * n_points + y * n_points + x][1] = double(y)/double(n_points);
              unit_points[z * n_points * n_points + y * n_points + x][2] = double(z)/double(n_points);
            }
      break;
    }

  const FE_Q<dim,spacedim> feq(2);
  const FESystem<dim,spacedim> fesystem(feq, spacedim);
  DoFHandler<dim,spacedim> dhq(triangulation);
  dhq.distribute_dofs(fesystem);
  Vector<double> eulerq(dhq.n_dofs());
  const ComponentMask mask(spacedim, true);

  VectorTools::get_position_vector(dhq, eulerq, mask);
  MappingFEField<dim,spacedim> map(dhq, eulerq, mask);

  typename Triangulation<dim, spacedim >::active_cell_iterator
  cell = triangulation.begin_active();

  for (unsigned int i=0; i<unit_points.size(); ++i)
    {
      // for each of the points,
      // verify that if we apply
      // the forward map and then
      // pull back that we get
      // the same point again
      const Point<spacedim> p = map.transform_unit_to_real_cell(cell,unit_points[i]);
      const Point<dim> p_unit = map.transform_real_to_unit_cell(cell,p);

      AssertThrow (unit_points[i].distance(p_unit) < 1e-10, ExcInternalError());
    }

  deallog << "OK" << std::endl;

}


int
main()
{
  std::ofstream logfile ("output");
  deallog.attach(logfile);
  deallog.threshold_double(1.e-10);

  test_real_to_unit_cell<1,1>();
  test_real_to_unit_cell<2,2>();
  test_real_to_unit_cell<3,3>();

  test_real_to_unit_cell<1,2>();
  test_real_to_unit_cell<2,3>();

  // test_real_to_unit_cell<1,3>();
  return 0;
}
