/**
 * @file dtnn_statistic.hpp
 * @author Ryan Curtin
 *
 * Statistic for dual-tree nearest neighbor search based k-means clustering.
 *
 * This file is part of mlpack 2.0.0.
 *
 * mlpack is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option) any
 * later version.
 *
 * mlpack is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details (LICENSE.txt).
 *
 * You should have received a copy of the GNU General Public License along with
 * mlpack.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MLPACK_METHODS_KMEANS_DTNN_STATISTIC_HPP
#define MLPACK_METHODS_KMEANS_DTNN_STATISTIC_HPP

#include <mlpack/methods/neighbor_search/neighbor_search_stat.hpp>

namespace mlpack {
namespace kmeans {

class DualTreeKMeansStatistic : public
    neighbor::NeighborSearchStat<neighbor::NearestNeighborSort>
{
 public:
  DualTreeKMeansStatistic() :
      neighbor::NeighborSearchStat<neighbor::NearestNeighborSort>(),
      upperBound(DBL_MAX),
      lowerBound(DBL_MAX),
      owner(size_t(-1)),
      pruned(size_t(-1)),
      staticPruned(false),
      staticUpperBoundMovement(0.0),
      staticLowerBoundMovement(0.0),
      centroid(),
      trueParent(NULL)
  {
    // Nothing to do.
  }

  template<typename TreeType>
  DualTreeKMeansStatistic(TreeType& node) :
      neighbor::NeighborSearchStat<neighbor::NearestNeighborSort>(),
      upperBound(DBL_MAX),
      lowerBound(DBL_MAX),
      owner(size_t(-1)),
      pruned(size_t(-1)),
      staticPruned(false),
      staticUpperBoundMovement(0.0),
      staticLowerBoundMovement(0.0),
      trueParent(node.Parent())
  {
    // Empirically calculate the centroid.
    centroid.zeros(node.Dataset().n_rows);
    for (size_t i = 0; i < node.NumPoints(); ++i)
    {
      // Correct handling of cover tree: don't double-count the point which
      // appears in the children.
      if (tree::TreeTraits<TreeType>::HasSelfChildren && i == 0 &&
          node.NumChildren() > 0)
        continue;
      centroid += node.Dataset().col(node.Point(i));
    }

    for (size_t i = 0; i < node.NumChildren(); ++i)
      centroid += node.Child(i).NumDescendants() *
          node.Child(i).Stat().Centroid();

    centroid /= node.NumDescendants();

    // Set the true children correctly.
    trueChildren.resize(node.NumChildren());
    for (size_t i = 0; i < node.NumChildren(); ++i)
      trueChildren[i] = &node.Child(i);
  }

  double UpperBound() const { return upperBound; }
  double& UpperBound() { return upperBound; }

  double LowerBound() const { return lowerBound; }
  double& LowerBound() { return lowerBound; }

  const arma::vec& Centroid() const { return centroid; }
  arma::vec& Centroid() { return centroid; }

  size_t Owner() const { return owner; }
  size_t& Owner() { return owner; }

  size_t Pruned() const { return pruned; }
  size_t& Pruned() { return pruned; }

  bool StaticPruned() const { return staticPruned; }
  bool& StaticPruned() { return staticPruned; }

  double StaticUpperBoundMovement() const { return staticUpperBoundMovement; }
  double& StaticUpperBoundMovement() { return staticUpperBoundMovement; }

  double StaticLowerBoundMovement() const { return staticLowerBoundMovement; }
  double& StaticLowerBoundMovement() { return staticLowerBoundMovement; }

  void* TrueParent() const { return trueParent; }
  void*& TrueParent() { return trueParent; }

  void* TrueChild(const size_t i) const { return trueChildren[i]; }
  void*& TrueChild(const size_t i) { return trueChildren[i]; }

  size_t NumTrueChildren() const { return trueChildren.size(); }

 private:
  double upperBound;
  double lowerBound;
  size_t owner;
  size_t pruned;
  bool staticPruned;
  double staticUpperBoundMovement;
  double staticLowerBoundMovement;
  arma::vec centroid;
  void* trueParent;
  std::vector<void*> trueChildren;
};

} // namespace kmeans
} // namespace mlpack

#endif
