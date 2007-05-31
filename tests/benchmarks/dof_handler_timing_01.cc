//----------------------------------------------------------------------
//    $Id$
//    Version: $Name$ 
//
//    Copyright (C) 2007 by the deal.II authors
//
//    This file is subject to QPL and may not be  distributed
//    without copyright and license information. Please refer
//    to the file deal.II/doc/license.html for the  text  and
//    further information on this license.
//
//----------------------------------------------------------------------

#include <iomanip>
#include <fstream>

#include <base/logstream.h>
#include <grid/tria.h>
#include <grid/grid_generator.h>
#include <dofs/dof_handler.h>
#include <dofs/dof_accessor.h>
#include <multigrid/mg_dof_handler.h>
#include <multigrid/mg_dof_accessor.h>
#include <fe/fe_q.h>
#include <fe/fe_dgq.h>
#include <fe/fe_system.h>

using namespace dealii;


// Fill dof handlers for different elements and see how large they get.
template <class DOF>
void check_dofs(DOF& dof, unsigned int repeat)
{
  FE_Q<DOF::dimension> q1(1);
  
  deallog << "Start" << std::endl;
  for (unsigned int i=1;i<6;++i)
    {
      FE_Q<DOF::dimension> fe(i);
      for (unsigned int i=0;i<repeat;++i)
	dof.distribute_dofs(fe);
      deallog << dof.get_fe().get_name()
	      << " memory " << dof.memory_consumption()/1.e6 << std::endl;
    }
  
  for (unsigned int i=1;i<6;++i)
    {
      FESystem<DOF::dimension> fe(q1, 1 << i);
      for (unsigned int i=0;i<repeat;++i)
	dof.distribute_dofs(fe);
      deallog << dof.get_fe().get_name()
	      << " memory " << dof.memory_consumption()/1.e6 << std::endl;
    }
  dof.clear();
}


template <int dim>
void check (bool local)
{
  deallog << "Start mesh"  << std::endl;
  Triangulation<dim> tr(Triangulation<dim>::maximum_smoothing);
  GridGenerator::hyper_cube(tr);

  if (local)
    for (unsigned int i=0;i<1800/dim;++i)
      {
	tr.last_active()->set_refine_flag();
	tr.execute_coarsening_and_refinement();
      }
  else
    tr.refine_global(21/dim);
  
  deallog << "Levels " << tr.n_levels() << " Cells "<< tr.n_cells()
	  << std::endl
	  << "active " << tr.n_active_cells()
	  << " memory " << tr.memory_consumption()/1.e6
	  << std::endl;
  
  deallog.push("DoF");
  DoFHandler<dim> dof(tr);
  check_dofs(dof, (local ? 10 : 1));
  deallog.pop();
  deallog.push("MGDoF");
  MGDoFHandler<dim> mgdof(tr);
  check_dofs(mgdof, (local ? 10 : 1));
  deallog.pop();
}

int main()
{
  std::ofstream out("dof_handler_timing_01/output");
  deallog.log_execution_time(true);
  deallog.log_time_differences(true);
  deallog.attach(out);
  deallog.push("local");
  deallog.push("2d");
  check<2>(true);
  deallog.pop();
  deallog.push("3d");
  check<3>(true);
  deallog.pop();
  deallog.pop();
  deallog.push("global");  
  deallog.push("2d");
  check<2>(false);
  deallog.pop();
  deallog.push("3d");
  check<3>(false);
  deallog.pop();
  deallog.pop();
}

