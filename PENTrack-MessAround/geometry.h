/**
 * \file
 * Contains class to include experiment geometry.
 */

#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include <string>
#include <fstream>
#include <vector>
#include <set>
#include <limits>
#include <map>

#include "trianglemesh.h"
#include "globals.h"

using namespace std;

static const double REFLECT_TOLERANCE = 1e-8;  ///< max distance of reflection point to actual surface collision point

/// Struct to store material properties (read from geometry.in, right now only for neutrons)
struct material{
	std::string name; ///< Material name
	double FermiReal; ///< Real part of Fermi potential
	double FermiImag; ///< Imaginary part of Fermi potential
	double DiffProb; ///< Diffuse reflection probability
	double SpinflipProb; ///< Probability for spin flip on reflection
	double RMSRoughness; ///< RMS roughness of surface, for MicroRoughness model reflections
	double CorrelLength; ///< Correlation length of surface roughness, for MicroRoughness model reflections
	bool UseMRModel; ///< Choose MicroRoughness model for reflections
};


/// Struct to store solid information (read from geometry.in)
struct solid{
	std::string name; ///< name of solid
	material mat; ///< material of solid
	unsigned ID; ///< ID of solid
	std::vector<double> ignoretimes; ///< pairs of times, between which the solid should be ignored

	/**
	 * Comparison operator used to sort solids by priority (descending)
	 *
	 * @param s solid struct compared to this solid
	 *
	 * @return Returns true if this solid's ID is larger than the other one's
	 */
	bool operator< (const solid s) const { return ID > s.ID; };
};


/**
 * Class to include experiment geometry.
 *
 * Loads solids and materials from geometry.in, maintains solids list, checks for collisions.
 */
struct TGeometry{
	public:
		TTriangleMesh mesh; ///< kd-tree structure containing triangle meshes from STL-files
		vector<solid> solids; ///< solids list
		solid defaultsolid; ///< "vacuum", this solid's properties are used when the particle is not inside any other solid
		
		/**
		 * Constructor, reads geometry configuration file, loads triangle meshes.
		 *
		 * @param geometryin TConfig struct containing MATERIALS and GEOMETRY config section
		 */
		TGeometry(TConfig &geometryin);


		/**
		 * Check if segment is intersecting with geometry bounding box.
		 *
		 * @param y1 Position vector of segment start
		 * @param y2 Position vector of segment end
		 *
		 * @return Returns true if segment is intersecting bounding box
		 */
		bool CheckSegment(const double y1[3], const double y2[3]){
			return CGAL::do_intersect(mesh.tree.bbox(), CSegment(CPoint(y1[0], y1[1], y1[2]), CPoint(y2[0], y2[1], y2[2])));
		};
		

		/**
		 * Checks if line segment p1->p2 collides with a surface.
		 *
		 * Calls KDTree::Collision to check for collisions and removes all collisions
		 * which should be ignored (given by ignore times in geometry configuration file).
		 *
		 * @param x1 Start time of line segment
		 * @param p1 Start point of line segment
		 * @param x2 End time of line segment
		 * @param p2 End point of line segment
		 * @param colls List of collisions, paired with bool indicator it it should be ignored
		 *
		 * @return Returns true if line segment collides with a surface
		 */
		bool GetCollisions(const double x1, const double p1[3], const double x2, const double p2[3], map<TCollision, bool> &colls);
		
			
		/**
		 * Get solids in which the point p lies
		 *
		 * @param t Time
		 * @param p Point to test
		 * @param currentsolids Map of solids in which the point is inside paired with information if it was ignored or not
		 */
		void GetSolids(const double t, const double p[3], map<solid, bool> &currentsolids);


		/**
		 * Get solid with highest priority in which the point p lies
		 *
		 * @param t Time
		 * @param p Point to test
		 *
		 * @return Returns solid with highest priority, that was not ignored at time t
		 */
		solid GetSolid(const double t, const double p[3]);


		/**
		 * Get solid with highest priority in which the point p lies
		 *
		 * @param t Time
		 * @param p Point to test
		 * @param currentsolids list of solids to choose from
		 *
		 * @return Returns solid with highest priority, that was not ignored at time t
		 */
		solid GetSolid(const double t, const double p[3], map<solid, bool> currentsolids);
};

#endif /*GEOMETRY_H_*/
