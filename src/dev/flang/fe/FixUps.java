/*

This file is part of the Fuzion language implementation.

The Fuzion language implementation is free software: you can redistribute it
and/or modify it under the terms of the GNU General Public License as published
by the Free Software Foundation, version 3 of the License.

The Fuzion language implementation is distributed in the hope that it will be
useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
License for more details.

You should have received a copy of the GNU General Public License along with The
Fuzion language implementation.  If not, see <https://www.gnu.org/licenses/>.

*/

/*-----------------------------------------------------------------------
 *
 * Tokiwa Software GmbH, Germany
 *
 * Source of class FixUps
 *
 *---------------------------------------------------------------------*/

package dev.flang.fe;

import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;

import dev.flang.ast.AbstractFeature;
import dev.flang.ast.AbstractType;
import dev.flang.ast.Feature;

import dev.flang.util.DataOut;
import dev.flang.util.SourcePosition;


/**
 * FixUps is a helper class for LibraryOut that extends DataOut with methods to
 * fix up indices and offsets that are known only after feature, types,
 * etc. were written.
 *
 * @author Fridtjof Siebert (siebert@tokiwa.software)
 */
class FixUps extends DataOut
{


  /*----------------------------  variables  ----------------------------*/


  /**
   * Features that are referenced before being defined and hence need a fixup:
   */
  private ArrayList<AbstractFeature> _fixUpsF = new ArrayList<>();


  /**
   * Positions of fixups for features
   */
  private ArrayList<Integer> _fixUpsFAt = new ArrayList<>();


  /**
   * Feature offsets in this file
   */
  private Map<AbstractFeature, Integer> _offsetsForFeature = new TreeMap<>();


  /**
   * Type offsets in this file
   */
  private Map<AbstractType, Integer> _offsetsForType = new TreeMap<>();


  /**
   * SourcePositions that need fixup.
   */
  private ArrayList<SourcePosition> _fixUpsSourcePositions = new ArrayList<>();


  /**
   * offsets of SourcePositions that need fixup.
   */
  private ArrayList<Integer> _fixUpsSourcePositionsAt = new ArrayList<>();


  /**
   * source file position offsets in this file.
   */
  private Map<String, Integer> _sourceFilePositions = new TreeMap<>();


  /*--------------------------  constructors  ---------------------------*/


  /**
   * Constructor to write library for given SourceModule.
   */
  FixUps()
  {
    super();
  }


  /*-----------------------------  methods  -----------------------------*/


  /**
   * Add current offset as the offset for feature f.
   *
   * @param f the feature that will be written immediately after this call.
   */
  void add(Feature f)
  {
    _offsetsForFeature.put(f, offset());
  }


  /**
   * get the number of features written.
   */
  int featureCount()
  {
    return _offsetsForFeature.size();
  }


  /**
   * Write offset of given feature, create fixup if not known yet.
   */
  void writeOffset(AbstractFeature f)
  {
    int v;
    if (f.isUniverse())
      {
        v = 0;
      }
    else if (f == null)
      {
        v = -1;
      }
    else
      {
        var o = _offsetsForFeature.get(f);
        if (o == null)
          {
            _fixUpsF.add(f);
            _fixUpsFAt.add(offset());
            v = -1;
          }
        else
          {
            v = (int) o;
          }
      }
    writeInt(v);
  }


  /*
   * Add fixup for source code position 'pos' at current offset.
   */
  void addSourcePosition(SourcePosition pos)
  {
    _fixUpsSourcePositions.add(pos);
    _fixUpsSourcePositionsAt.add(offset());
  }


  /*
   * Add current offset as position for given source file
   */
  void addSourceFilePosition(String n)
  {
    _sourceFilePositions.put(n, offset());
  }


  /**
   * Perform fixups
   */
  void fixUps(LibraryOut lo)
  {
    for (var i = 0; i<_fixUpsF.size(); i++)
      {
        var g  = _fixUpsF  .get(i);
        var at = _fixUpsFAt.get(i);
        var o = _offsetsForFeature.get(g);
        if (o == null)
          {
            if (g instanceof LibraryFeature gl)
              {
                System.out.println("Writing offset for " + g.qualifiedName() + " from " + gl._libModule + "@" + gl._index + " ==> "+offset()+"+"+gl._index);
              }
            else
              {
                System.out.println("NYI: writing module depending on other module not supported yet, missing offset for " + g.qualifiedName() + "!");
              }
          }
        else
          {
            if (CHECKS) check
              (o != null);
            writeIntAt(at, o);
          }
      }
    for (var i = 0; i<_fixUpsSourcePositions.size(); i++)
      {
        var p  = _fixUpsSourcePositions  .get(i);
        var at = _fixUpsSourcePositionsAt.get(i);
        var sf = p._sourceFile;
        var n = lo.fileName(sf);
        var o = _sourceFilePositions.get(n) + p.bytePos();
        if (CHECKS) check
          (o > 0);
        writeIntAt(at, o);
      }
  }


  /**
   * Record offset as the offset of type t.
   *
   * @param t a type that was or will be written out
   *
   * @param offset of t in the offset in the .fum/MIR file
   */
  void addOffset(AbstractType t, int offset)
  {
    if (PRECONDITIONS) require
      (offset(t) == -1);

    _offsetsForType.put(t, offset);
  }


  /**
   * Get the offset that was previously recored for type t, or -1 if no offset
   * was record (i.e., t has not been written yet).
   */
  int offset(AbstractType t)
  {
    return _offsetsForType.getOrDefault(t, -1);
  }


  /**
   * get the number of types added
   */
  int typeCount()
  {
    return _offsetsForType.size();
  }

}

/* end of file */
