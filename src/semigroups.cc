/*
 * semigroups: Methods for semigroups
 */

#include "semigroups.h"
#include "elements.h"
#include <unordered_map>
#include <iostream>
#include <assert.h>

//using namespace semigroups;
//using Transformation = semigroups::Transformation;

//class Element; 
//class Transformation; 
//Transformation* x = new Transformation(image);

//#define DEBUG 1

class RecVec {
        
      public:
        
        RecVec (const RecVec& copy) = delete;
        RecVec& operator= (RecVec const& copy) = delete;
        
        RecVec(size_t nrcols, size_t nrrows=0) : _vec(new std::vector<u_int32_t>()), 
                                                 _nrcols(nrcols),
                                                 _nrrows(0){
          std::cout << "constructed!!\n";
          this->expand(nrrows);
        }
        
        ~RecVec() {
          std::cout << "destructed!!\n";
          delete _vec;
        }
        
        u_int32_t get (size_t i, size_t j) {
          assert(i - 1 < _nrrows && j - 1 < _nrcols);
          return _vec->at((i - 1) * _nrcols + (j - 1)); 
        }
        
        void set (size_t i, size_t j, u_int32_t val) {
          assert(i - 1 < _nrrows && j - 1 < _nrcols);
          _vec->at((i - 1) * _nrcols + (j - 1)) = val; 
        }

        void expand (size_t nr=1) {
          _nrrows += nr;
          for (size_t i = 0; i < _nrcols * nr; i++) {
            _vec->push_back(0);
          }
        }

        size_t size () {
          return _vec->size();
        }

      private:

        std::vector<u_int32_t>* _vec;
        size_t _nrcols;
        size_t _nrrows;
};

//class GenericData {
//
//};

void SemigroupsFreeFunc(Obj o) { 
  std::cout << "freeing!!\n";
  delete GET_WRAP<RecVec>(o);
}

Obj NewWrap(RecVec* rv){
    Obj o = NewBag(T_SEMI, 1 * sizeof(Obj));
    SET_WRAP<RecVec>(o, rv);
    return o;
}



extern "C" {
  #include "src/compiled.h"          /* GAP headers                */
  Obj HTValue_TreeHash_C ( Obj self, Obj ht, Obj obj );
  Obj HTAdd_TreeHash_C ( Obj self, Obj ht, Obj obj, Obj val);
}


#define ELM_PLIST2(plist, i, j)       ELM_PLIST(ELM_PLIST(plist, i), j)
#define INT_PLIST(plist, i)           INT_INTOBJ(ELM_PLIST(plist, i))
#define INT_PLIST2(plist, i, j)       INT_INTOBJ(ELM_PLIST2(plist, i, j))

static void SET_ELM_PLIST2(Obj plist, UInt i, UInt j, Obj val) {
  SET_ELM_PLIST(ELM_PLIST(plist, i), j, val);
  SET_LEN_PLIST(ELM_PLIST(plist, i), j);
  CHANGED_BAG(plist);
}

// assumes the length of data!.elts is at most 2^28

Obj ENUMERATE_SEE_DATA (Obj self, Obj data, Obj limit, Obj lookfunc, Obj looking) {
  Obj   found, elts, gens, genslookup, left, first, final, prefix, suffix, 
        reduced, words, ht, rules, lenindex, newElt, newword, objval, newrule,
        empty, oldword, x;
  UInt  i, nr, len, stopper, nrrules, b, s, r, p, j, k, int_limit, nrgens,
        intval, stop, one;

  std::vector<u_int16_t> image;
  Transformation* xxx = new Transformation(image);
  std::unordered_map<Transformation, u_int16_t> map;
  map.insert(std::make_pair(xxx, 1)); 
  
  delete xxx;
  //remove nrrules 
  if(looking==True){
    AssPRec(data, RNamName("found"), False);
  }

  int_limit = INT_INTOBJ(limit);
  i = INT_INTOBJ(ElmPRec(data, RNamName("pos")));
  // TODO check the position and init the data
  nr = INT_INTOBJ(ElmPRec(data, RNamName("nr")));

  if(i>nr){
    return data;
  }
  #ifdef DEBUG
    Pr("here 1\n", 0L, 0L);
  #endif 
  // get everything out of <data>
  
  //lists of integers, objects
  elts = ElmPRec(data, RNamName("elts")); 
  // the so far enumerated elements
  gens = ElmPRec(data, RNamName("gens")); 
  nrgens=LEN_PLIST(gens);
  // the generators
  genslookup = ElmPRec(data, RNamName("genslookup"));     
  // genslookup[i]=Position(elts, gens[i], this is not always <i+1>!
  lenindex = ElmPRec(data, RNamName("lenindex"));         
  // lenindex[len]=position in <words> and <elts> of first element of length
  // <len> 
  first = ElmPRec(data, RNamName("first"));
  // elts[i]=gens[first[i]]*elts[suffix[i]], first letter 
  final = ElmPRec(data, RNamName("final"));
  // elts[i]=elts[prefix[i]]*gens[final[i]]
  prefix = ElmPRec(data, RNamName("prefix"));
  // see final, 0 if prefix is empty i.e. elts[i] is a gen
  suffix = ElmPRec(data, RNamName("suffix"));             
  // see first, 0 if suffix is empty i.e. elts[i] is a gen
  
  #ifdef DEBUG 
    Pr("here 2\n", 0L, 0L); 
  #endif 

  RecVec* right;
  if (i == 1) {
    right = new RecVec(nrgens, nrgens);
    AssPRec(data, RNamName("right"), NewWrap(right));
  } else {
    right = GET_WRAP<RecVec>(ElmPRec(data, RNamName("right")));
  }
  //lists of lists
  //right = ElmPRec(data, RNamName("right")); 
  // elts[right[i][j]]=elts[i]*gens[j], right Cayley graph
  left = ElmPRec(data, RNamName("left"));                 
  // elts[left[i][j]]=gens[j]*elts[i], left Cayley graph
  reduced = ElmPRec(data, RNamName("reduced"));           
  // words[right[i][j]] is reduced if reduced[i][j]=true
  words = ElmPRec(data, RNamName("words"));
  // words[i] is a word in the gens equal to elts[i]
  rules = ElmPRec(data, RNamName("rules"));               
  if(TNUM_OBJ(rules)==T_PLIST_EMPTY){
    RetypeBag(rules, T_PLIST_CYC);
  }
  // the relations
 
  //hash table
  ht = ElmPRec(data, RNamName("ht"));                     
  // HTValue(ht, x)=Position(elts, x)
 
  #ifdef DEBUG
    Pr("here 3\n", 0L, 0L);
  #endif 
  //integers
  len=INT_INTOBJ(ElmPRec(data, RNamName("len"))); 
  // current word length
  if(IS_INTOBJ(ElmPRec(data, RNamName("one")))){
    one=INT_INTOBJ(ElmPRec(data, RNamName("one")));
  } else {
    one=0;
  }
  // <elts[one]> is the mult. neutral element
  stopper=INT_INTOBJ(ElmPRec(data, RNamName("stopper")));
  // stop when we have applied generators to elts[stopper] 
  nrrules=INT_INTOBJ(ElmPRec(data, RNamName("nrrules")));           
  // Length(rules)
  
  stop = 0;
  found = False;

  #ifdef DEBUG
    Pr("here 4\n", 0L, 0L);
  #endif 

  while(i<=nr&&!stop){
    while(i<=nr&&LEN_PLIST(ELM_PLIST(words, i))==len&&!stop){
      b=INT_INTOBJ(ELM_PLIST(first, i)); 
      s=INT_INTOBJ(ELM_PLIST(suffix, i));
      //RetypeBag(ELM_PLIST(right, i), T_PLIST_CYC); //from T_PLIST_EMPTY
      for(j = 1;j <= nrgens;j++){
        #ifdef DEBUG
          Pr("i=%d\n", (Int) i, 0L);
          Pr("j=%d\n", (Int) j, 0L);
        #endif
        if(s != 0&&ELM_PLIST2(reduced, s, j) == False){
          r = right->get(s, j);
          //r = INT_PLIST2(right, s, j);
          if(INT_PLIST(prefix, r) != 0){
            #ifdef DEBUG
              Pr("Case 1!\n", 0L, 0L);
            #endif
            intval = INT_PLIST2(left, INT_PLIST(prefix, r), b);
            //SET_ELM_PLIST2(right, i, j, ELM_PLIST2(right, intval, INT_PLIST(final, r)));
            right->set(i, j, right->get(intval, INT_PLIST(final, r)));
            SET_ELM_PLIST2(reduced, i, j, False);
          } else if (r == one){
            #ifdef DEBUG
              Pr("Case 2!\n", 0L, 0L);
            #endif
            //SET_ELM_PLIST2(right, i, j, ELM_PLIST(genslookup, b));
            right->set(i, j, INT_PLIST(genslookup, b));
            SET_ELM_PLIST2(reduced, i, j, False);
          } else {
            #ifdef DEBUG
              Pr("Case 3!\n", 0L, 0L);
            #endif
            right->set(i, j, right->get(INT_PLIST(genslookup, b), INT_PLIST(final, r)));
            //SET_ELM_PLIST2(right, i, j, 
            //  ELM_PLIST2(right, INT_PLIST(genslookup, b), INT_PLIST(final, r)));
            SET_ELM_PLIST2(reduced, i, j, False);
          }
        } else {
          newElt = PROD(ELM_PLIST(elts, i), ELM_PLIST(gens, j)); 
          oldword = ELM_PLIST(words, i);
          len = LEN_PLIST(oldword);
          newword = NEW_PLIST(T_PLIST_CYC, len+1);

          memcpy( (void *)((char *)(ADDR_OBJ(newword)) + sizeof(Obj)), 
                  (void *)((char *)(ADDR_OBJ(oldword)) + sizeof(Obj)), 
                  (size_t)(len*sizeof(Obj)) );
          SET_ELM_PLIST(newword, len+1, INTOBJ_INT(j));
          SET_LEN_PLIST(newword, len+1);

          objval = HTValue_TreeHash_C(self, ht, newElt); 
          if(objval!=Fail){
            #ifdef DEBUG
              Pr("Case 4!\n", 0L, 0L);
            #endif
            newrule = NEW_PLIST(T_PLIST, 2);
            SET_ELM_PLIST(newrule, 1, newword);
            SET_ELM_PLIST(newrule, 2, ELM_PLIST(words, INT_INTOBJ(objval)));
            SET_LEN_PLIST(newrule, 2);
            CHANGED_BAG(newrule);
            nrrules++;
            AssPlist(rules, nrrules, newrule);
            right->set(i, j, INT_INTOBJ(objval));
            //SET_ELM_PLIST2(right, i, j, objval);
          } else {
            #ifdef DEBUG
              Pr("Case 5!\n", 0L, 0L);
            #endif
            nr++;
            
            HTAdd_TreeHash_C(self, ht, newElt, INTOBJ_INT(nr));

            if(one==0){
              one=nr;
              for(k=1;k<=nrgens;k++){
                x = ELM_PLIST(gens, k);
                if(!EQ(PROD(newElt, x), x)){
                  one=0;
                  break;
                }
                if(!EQ(PROD(x, newElt), x)){
                  one=0;
                  break;
                }
              }
            }

            if(s!=0){
              AssPlist(suffix, nr, INTOBJ_INT(right->get(s, j)));
                //ELM_PLIST2(right, s, j));
            } else {
              AssPlist(suffix, nr, ELM_PLIST(genslookup, j));
            }
          
            AssPlist(elts, nr, newElt);
            AssPlist(words, nr, newword);
            AssPlist(first, nr, INTOBJ_INT(b));
            AssPlist(final, nr, INTOBJ_INT(j));
            AssPlist(prefix, nr, INTOBJ_INT(i));
            
            //empty = NEW_PLIST(T_PLIST_EMPTY, nrgens);
            //SET_LEN_PLIST(empty, 0);
            //AssPlist(right, nr, empty);
            right->expand();
            
            empty = NEW_PLIST(T_PLIST_EMPTY, nrgens);
            SET_LEN_PLIST(empty, 0);
            AssPlist(left, nr, empty);
            
            empty = NEW_PLIST(T_PLIST_CYC, nrgens);
            for(k=1;k<=nrgens;k++){
              SET_ELM_PLIST(empty, k, False);
            }
            SET_LEN_PLIST(empty, nrgens);
            AssPlist(reduced, nr, empty);
            
            SET_ELM_PLIST2(reduced, i, j, True);
            //SET_ELM_PLIST2(right, i, j, INTOBJ_INT(nr));
            right->set(i, j, nr);
            
            if(looking==True&&found==False&&
                CALL_2ARGS(lookfunc, data, INTOBJ_INT(nr))==True){
                found=True;
                stop=1;
                AssPRec(data,  RNamName("found"), INTOBJ_INT(nr)); 
            } else {
              stop=(nr>=int_limit);
            }
          }
        }
      }//finished applying gens to <elts[i]>
      stop=(stop||i==stopper);
      i++;
    }//finished words of length <len> or <stop>
    #ifdef DEBUG
      Pr("finished processing words of len %d\n", (Int) len, 0L);
    #endif
    if(i>nr||LEN_PLIST(ELM_PLIST(words, i))!=len){
      if(len>1){
        #ifdef DEBUG
          Pr("Case 6!\n", 0L, 0L);
        #endif
        for(j=INT_INTOBJ(ELM_PLIST(lenindex, len));j<=i-1;j++){ 
          RetypeBag(ELM_PLIST(left, j), T_PLIST_CYC); //from T_PLIST_EMPTY
          p=INT_INTOBJ(ELM_PLIST(prefix, j)); 
          b=INT_INTOBJ(ELM_PLIST(final, j));
          for(k=1;k<=nrgens;k++){ 
            SET_ELM_PLIST2(left, j, k, INTOBJ_INT(right->get(INT_PLIST2(left, p, k), b)));
                //ELM_PLIST2(right, INT_PLIST2(left, p, k), b));
          }
        }
      } else if(len==1){ 
        #ifdef DEBUG
          Pr("Case 7!\n", 0L, 0L);
        #endif
        for(j=INT_INTOBJ(ELM_PLIST(lenindex, len));j<=i-1;j++){ 
          RetypeBag(ELM_PLIST(left, j), T_PLIST_CYC); //from T_PLIST_EMPTY
          b=INT_INTOBJ(ELM_PLIST(final, j));
          for(k=1;k<=nrgens;k++){ 
            SET_ELM_PLIST2(left, j, k, INTOBJ_INT(right->get(INT_PLIST(genslookup, k), b)));
                //ELM_PLIST2(right, INT_PLIST(genslookup, k) , b));
          }
        }
      }
      len++;
      AssPlist(lenindex, len, INTOBJ_INT(i));  
    }
  }
  if (i > nr) {
    // free stuff
  }
  AssPRec(data, RNamName("nr"), INTOBJ_INT(nr));  
  AssPRec(data, RNamName("nrrules"), INTOBJ_INT(nrrules));  
  AssPRec(data, RNamName("one"), ((one!=0)?INTOBJ_INT(one):False));  
  AssPRec(data, RNamName("pos"), INTOBJ_INT(i));  
  AssPRec(data, RNamName("len"), INTOBJ_INT(len));  

  CHANGED_BAG(data); 
  //SemigroupsFreeFunc(ElmPRec(data, RNamName("right")));
  return data;
}

/****************************************************************************
**
*F  FuncGABOW_SCC
**
** `digraph' should be a list whose entries and the lists of out-neighbours
** of the vertices. So [[2,3],[1],[2]] represents the graph whose edges are
** 1->2, 1->3, 2->1 and 3->2.
**
** returns a newly constructed record with two components 'comps' and 'id' the
** elements of 'comps' are lists representing the strongly connected components
** of the directed graph, and in the component 'id' the following holds:
** id[i]=PositionProperty(comps, x-> i in x);
** i.e. 'id[i]' is the index in 'comps' of the component containing 'i'.  
** Neither the components, nor their elements are in any particular order.
**
** The algorithm is that of Gabow, based on the implementation in Sedgwick:
**   http://algs4.cs.princeton.edu/42directed/GabowSCC.java.html
** (made non-recursive to avoid problems with stack limits) and 
** the implementation of STRONGLY_CONNECTED_COMPONENTS_DIGRAPH in listfunc.c.
*/

Obj GABOW_SCC(Obj self, Obj digraph) {
  UInt end1, end2, count, level, w, v, n, nr, idw, *frames, *stack2;
  Obj  id, stack1, out, comp, comps, adj; 
 
  PLAIN_LIST(digraph);
  n = LEN_PLIST(digraph);
  if (n == 0){
    out = NEW_PREC(2);
    AssPRec(out, RNamName("id"), NEW_PLIST(T_PLIST_EMPTY+IMMUTABLE,0));
    AssPRec(out, RNamName("comps"), NEW_PLIST(T_PLIST_EMPTY+IMMUTABLE,0));
    CHANGED_BAG(out);
    return out;
  }

  end1 = 0; 
  stack1 = NEW_PLIST(T_PLIST_CYC, n); 
  //stack1 is a plist so we can use memcopy below
  SET_LEN_PLIST(stack1, n);
  
  id = NEW_PLIST(T_PLIST_CYC+IMMUTABLE, n);
  SET_LEN_PLIST(id, n);
  
  //init id
  for(v=1;v<=n;v++){
    SET_ELM_PLIST(id, v, INTOBJ_INT(0));
  }
  
  count = n;
  
  comps = NEW_PLIST(T_PLIST_TAB+IMMUTABLE, n);
  SET_LEN_PLIST(comps, 0);
  
  stack2 = static_cast<UInt*>(malloc( (4 * n + 2) * sizeof(UInt) ));
  frames = stack2 + n + 1;
  end2 = 0;
  
  for (v = 1; v <= n; v++) {
    if(INT_INTOBJ(ELM_PLIST(id, v)) == 0){
      adj =  ELM_PLIST(digraph, v);
      PLAIN_LIST(adj);
      level = 1;
      frames[0] = v; // vertex
      frames[1] = 1; // index
      frames[2] = (UInt)adj; 
      SET_ELM_PLIST(stack1, ++end1, INTOBJ_INT(v));
      stack2[++end2] = end1;
      SET_ELM_PLIST(id, v, INTOBJ_INT(end1)); 
      
      while (1) {
        if (frames[1] > (UInt) LEN_PLIST(frames[2])) {
          if (stack2[end2] == (UInt) INT_INTOBJ(ELM_PLIST(id, frames[0]))) {
            end2--;
            count++;
            nr = 0;
            do{
              nr++;
              w = INT_INTOBJ(ELM_PLIST(stack1, end1--));
              SET_ELM_PLIST(id, w, INTOBJ_INT(count));
            }while (w != frames[0]);
            
            comp = NEW_PLIST(T_PLIST_CYC+IMMUTABLE, nr);
            SET_LEN_PLIST(comp, nr);
           
            memcpy( (void *)((char *)(ADDR_OBJ(comp)) + sizeof(Obj)), 
                    (void *)((char *)(ADDR_OBJ(stack1)) + (end1 + 1) * sizeof(Obj)), 
                    (size_t)(nr * sizeof(Obj)));

            nr = LEN_PLIST(comps) + 1;
            SET_ELM_PLIST(comps, nr, comp);
            SET_LEN_PLIST(comps, nr);
            CHANGED_BAG(comps);
          }
          level--;
          if (level == 0) {
            break;
          }
          frames -= 3;
        } else {
          
          w = INT_INTOBJ(ELM_PLIST(frames[2], frames[1]++));
          idw = INT_INTOBJ(ELM_PLIST(id, w));
          
          if(idw==0){
            adj = ELM_PLIST(digraph, w);
            PLAIN_LIST(adj);
            level++;
            frames += 3; 
            frames[0] = w; // vertex
            frames[1] = 1; // index
            frames[2] = (UInt) adj;
            SET_ELM_PLIST(stack1, ++end1, INTOBJ_INT(w));
            stack2[++end2] = end1;
            SET_ELM_PLIST(id, w, INTOBJ_INT(end1)); 
          } else {
            while (stack2[end2] > idw) {
              end2--; // pop from stack2
            }
          }
        }
      }
    }
  }

  for (v = 1; v <= n; v++) {
    SET_ELM_PLIST(id, v, INTOBJ_INT(INT_INTOBJ(ELM_PLIST(id, v)) - n));
  }

  out = NEW_PREC(2);
  SHRINK_PLIST(comps, LEN_PLIST(comps));
  AssPRec(out, RNamName("id"), id);
  AssPRec(out, RNamName("comps"), comps);
  CHANGED_BAG(out);
  free(stack2);
  return out;
}

// using the output of GABOW_SCC on the right and left Cayley graphs of a
// semigroup, the following function calculates the strongly connected
// components of the union of these two graphs.

Obj SCC_UNION_LEFT_RIGHT_CAYLEY_GRAPHS(Obj self, Obj scc1, Obj scc2)
{ UInt  n, len, nr, i, j, k, l, *ptr;
  Obj   comps1, id2, comps2, id, comps, seen, comp1, comp2, new_comp, x, out;

  n = LEN_PLIST(ElmPRec(scc1, RNamName("id")));
  
  if (n == 0){
    out = NEW_PREC(2);
    AssPRec(out, RNamName("id"), NEW_PLIST(T_PLIST_EMPTY+IMMUTABLE,0));
    AssPRec(out, RNamName("comps"), NEW_PLIST(T_PLIST_EMPTY+IMMUTABLE,0));
    CHANGED_BAG(out);
    return out;
  }

  comps1 = ElmPRec(scc1, RNamName("comps"));
  id2 = ElmPRec(scc2, RNamName("id"));
  comps2 = ElmPRec(scc2, RNamName("comps"));
   
  id = NEW_PLIST(T_PLIST_CYC+IMMUTABLE, n);
  SET_LEN_PLIST(id, n);
  seen = NewBag(T_DATOBJ, (LEN_PLIST(comps2)+1)*sizeof(UInt));
  ptr = (UInt *)ADDR_OBJ(seen);
  //init id
  for(i=1;i<=n;i++){
    SET_ELM_PLIST(id, i, INTOBJ_INT(0));
    ptr[i] = 0;
  }
  
  comps = NEW_PLIST(T_PLIST_TAB+IMMUTABLE, LEN_PLIST(comps1));
  SET_LEN_PLIST(comps, 0);
 
  nr = 0;
  
  for(i=1;i<=LEN_PLIST(comps1);i++){
    comp1 = ELM_PLIST(comps1, i);
    if(INT_INTOBJ(ELM_PLIST(id, INT_INTOBJ(ELM_PLIST(comp1, 1))))==0){
      nr++;
      new_comp = NEW_PLIST(T_PLIST_CYC+IMMUTABLE, LEN_PLIST(comp1));
      SET_LEN_PLIST(new_comp, 0);
      for(j=1;j<=LEN_PLIST(comp1);j++){
        k=INT_INTOBJ(ELM_PLIST(id2, INT_INTOBJ(ELM_PLIST(comp1, j))));
        if((UInt *)ADDR_OBJ(seen)[k]==0){
          ((UInt *)ADDR_OBJ(seen))[k]=1;
          comp2 = ELM_PLIST(comps2, k);
          for(l=1;l<=LEN_PLIST(comp2);l++){
            x=ELM_PLIST(comp2, l);
            SET_ELM_PLIST(id, INT_INTOBJ(x), INTOBJ_INT(nr));
            len=LEN_PLIST(new_comp);
            AssPlist(new_comp, len+1, x);
            SET_LEN_PLIST(new_comp, len+1);
          }
        }
      }
      SHRINK_PLIST(new_comp, LEN_PLIST(new_comp));
      len=LEN_PLIST(comps)+1;
      SET_ELM_PLIST(comps, len, new_comp);
      SET_LEN_PLIST(comps, len);
      CHANGED_BAG(comps);
    }
  }

  out = NEW_PREC(2);
  SHRINK_PLIST(comps, LEN_PLIST(comps));
  AssPRec(out, RNamName("id"), id);
  AssPRec(out, RNamName("comps"), comps);
  CHANGED_BAG(out);
  return out;
}

// <right> and <left> should be scc data structures for the right and left
// Cayley graphs of a semigroup, as produced by GABOW_SCC. This function find
// the H-classes of the semigroup from <right> and <left>. The method used is
// that described in:
// http://www.liafa.jussieu.fr/~jep/PDF/Exposes/StAndrews.pdf

Obj FIND_HCLASSES(Obj self, Obj right, Obj left){ 
  UInt  n, nrcomps, i, hindex, rindex, init, j, k, len, *nextpos, *sorted, *lookup;
  Obj   rightid, leftid, comps, buf, id, out, comp;
  
  rightid = ElmPRec(right, RNamName("id"));
  leftid = ElmPRec(left, RNamName("id"));
  n = LEN_PLIST(rightid);
  
  if (n == 0){
    out = NEW_PREC(2);
    AssPRec(out, RNamName("id"), NEW_PLIST(T_PLIST_EMPTY+IMMUTABLE,0));
    AssPRec(out, RNamName("comps"), NEW_PLIST(T_PLIST_EMPTY+IMMUTABLE,0));
    CHANGED_BAG(out);
    return out;
  }
  comps = ElmPRec(right, RNamName("comps"));
  nrcomps = LEN_PLIST(comps);
  
  buf = NewBag(T_DATOBJ, (2*n+nrcomps+1)*sizeof(UInt));
  nextpos = (UInt *)ADDR_OBJ(buf);
  
  nextpos[1] = 1;
  for(i=2;i<=nrcomps;i++){
    nextpos[i]=nextpos[i-1]+LEN_PLIST(ELM_PLIST(comps, i-1));
  }
  
  sorted = (UInt *)ADDR_OBJ(buf) + nrcomps;
  lookup = (UInt *)ADDR_OBJ(buf) + nrcomps + n;
  for(i = 1;i <= n; i++){
    j = INT_INTOBJ(ELM_PLIST(rightid, i));
    sorted[nextpos[j]] = i;
    nextpos[j]++;
    lookup[i] = 0;
  }
  
  id = NEW_PLIST(T_PLIST_CYC+IMMUTABLE, n);
  SET_LEN_PLIST(id, n);
  comps = NEW_PLIST(T_PLIST_TAB+IMMUTABLE, n);
  SET_LEN_PLIST(comps, 0);
  
  sorted = (UInt *)ADDR_OBJ(buf) + nrcomps;
  lookup = (UInt *)ADDR_OBJ(buf) + nrcomps + n;

  hindex = 0;
  rindex = 0;
  init = 0;
  
  for(i=1;i<=n;i++){
    j = sorted[i];
    k = INT_INTOBJ(ELM_PLIST(rightid, j));
    if(k > rindex){
      rindex = k;
      init = hindex;
    }
    k = INT_INTOBJ(ELM_PLIST(leftid, j));
    if(lookup[k]<=init){
      hindex++;
      lookup[k] = hindex;

      comp = NEW_PLIST(T_PLIST_CYC+IMMUTABLE, 1);
      SET_LEN_PLIST(comp, 0);
      SET_ELM_PLIST(comps, hindex, comp);
      SET_LEN_PLIST(comps, hindex);
      CHANGED_BAG(comps);

      sorted = (UInt *)ADDR_OBJ(buf) + nrcomps;
      lookup = (UInt *)ADDR_OBJ(buf) + nrcomps + n;
    }
    k = lookup[k];
    comp = ELM_PLIST(comps, k);
    len = LEN_PLIST(comp) + 1;
    AssPlist(comp, len, INTOBJ_INT(j)); 
    SET_LEN_PLIST(comp, len);
    
    SET_ELM_PLIST(id, j, INTOBJ_INT(k));
  }

  SHRINK_PLIST(comps, LEN_PLIST(comps));
  for(i=1;i<=LEN_PLIST(comps);i++){
    comp = ELM_PLIST(comps, i);
    SHRINK_PLIST(comp, LEN_PLIST(comp));
  }
  
  out = NEW_PREC(2);
  AssPRec(out, RNamName("id"), id);
  AssPRec(out, RNamName("comps"), comps);
  CHANGED_BAG(out);

  return out;
}

/*****************************************************************************/

typedef Obj (* GVarFunc)(/*arguments*/);

#define GVAR_FUNC_TABLE_ENTRY(srcfile, name, nparam, params) \
  {#name, nparam, \
   params, \
   (GVarFunc)name, \
   srcfile ":Func" #name }

// Table of functions to export
static StructGVarFunc GVarFuncs [] = {
    GVAR_FUNC_TABLE_ENTRY("semigroups.c", ENUMERATE_SEE_DATA, 4, 
                          "data, limit, lookfunc, looking"),
    GVAR_FUNC_TABLE_ENTRY("semigroups.c", GABOW_SCC, 1, 
                          "digraph"),
    GVAR_FUNC_TABLE_ENTRY("semigroups.c", SCC_UNION_LEFT_RIGHT_CAYLEY_GRAPHS, 2, 
                          "scc1, scc2"),
    GVAR_FUNC_TABLE_ENTRY("semigroups.c", FIND_HCLASSES, 2, 
                          "left, right"),

	{ 0 } /* Finish with an empty entry */

};

/******************************************************************************
*F  InitKernel( <module> )  . . . . . . . . initialise kernel data structures
*/
static Int InitKernel( StructInitInfo *module )
{
    /* init filters and functions                                          */
    InitHdlrFuncsFromTable( GVarFuncs );
    InfoBags[T_SEMI].name = "Semigroups package C++ type";
    InitMarkFuncBags(T_SEMI, &MarkNoSubBags);
    InitFreeFuncBag(T_SEMI, &SemigroupsFreeFunc);
    
    /* return success                                                      */
    return 0;
}

/******************************************************************************
*F  InitLibrary( <module> ) . . . . . . .  initialise library data structures
*/
static Int InitLibrary( StructInitInfo *module )
{
    /* init filters and functions */
    InitGVarFuncsFromTable( GVarFuncs );

    /* return success                                                      */
    return 0;
}

/******************************************************************************
*F  InitInfopl()  . . . . . . . . . . . . . . . . . table of init functions
*/
static StructInitInfo module = {
 /* type        = */ MODULE_DYNAMIC,
 /* name        = */ "semigroups",
 /* revision_c  = */ 0,
 /* revision_h  = */ 0,
 /* version     = */ 0,
 /* crc         = */ 0,
 /* initKernel  = */ InitKernel,
 /* initLibrary = */ InitLibrary,
 /* checkInit   = */ 0,
 /* preSave     = */ 0,
 /* postSave    = */ 0,
 /* postRestore = */ 0
};

extern "C"
StructInitInfo * Init__Dynamic ( void )
{
  return &module;
}
