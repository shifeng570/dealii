//----------------------------  petsc_vector_print.cc  ---------------------------
//    $Id$
//    Version: $Name$ 
//
//    Copyright (C) 2004, 2005, 2013 by the deal.II authors
//
//    This file is subject to QPL and may not be  distributed
//    without copyright and license information. Please refer
//    to the file deal.II/doc/license.html for the  text  and
//    further information on this license.
//
//----------------------------  petsc_vector_print.cc  ---------------------------


// verify that VectorBase::print uses the precision parameter correctly and
// restores the previous value of the stream precision

#include "../tests.h"
#include <deal.II/lac/petsc_vector.h>    
#include <fstream>
#include <iostream>
#include <vector>


void test (PETScWrappers::Vector &v,
           PETScWrappers::Vector &w)
{
                                   // set the first vector
  for (unsigned int i=0; i<v.size(); ++i)
    v(i) = i;

                                   // copy elements by reference
  for (unsigned int i=0; i<v.size(); ++i)
    w(i) = v(i);

                                   // check that they're equal
  Assert (v==w, ExcInternalError());

  deallog << "OK" << std::endl;
}



int main (int argc, char **argv)
{
  std::ofstream logfile("vector_print/output");
  deallog.attach(logfile);
  deallog.depth_console(0);
  deallog.threshold_double(1.e-10);

  try
    {
      Utilities::MPI::MPI_InitFinalize mpi_initialization(argc, argv, 1);
      {
        PETScWrappers::Vector v (5);
	for (unsigned int i=0; i<v.size(); ++i)
	  v(i) = i*1.2345678901234567;

	// print with old precision
	deallog << numbers::PI << std::endl;

	// print with prescribed precision
	deallog << "across=false" << std::endl;
	v.print (logfile, 10, true, false);
	
	deallog << "across=true" << std::endl;
	v.print (logfile, 10, true, true);

	// print once more. should be the old precision again
	deallog << numbers::PI << std::endl;
      }
      
    }
  catch (std::exception &exc)
    {
      std::cerr << std::endl << std::endl
		<< "----------------------------------------------------"
		<< std::endl;
      std::cerr << "Exception on processing: " << std::endl
		<< exc.what() << std::endl
		<< "Aborting!" << std::endl
		<< "----------------------------------------------------"
		<< std::endl;
      
      return 1;
    }
  catch (...) 
    {
      std::cerr << std::endl << std::endl
		<< "----------------------------------------------------"
		<< std::endl;
      std::cerr << "Unknown exception!" << std::endl
		<< "Aborting!" << std::endl
		<< "----------------------------------------------------"
		<< std::endl;
      return 1;
    };
}