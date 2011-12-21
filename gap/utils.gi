#############################################################################
##
#W  utils.gi
#Y  Copyright (C) 2011                                   James D. Mitchell
##
##  Licensing information can be found in the README file of this package.
##
#############################################################################
##

# this file contains functions with shorter names than some library functions
# commonly used in the citrus package. 

# mod for 0.4! - CitrusMakeDoc - "for no argument"
#############################################################################

InstallGlobalFunction(CitrusMakeDoc, 
function()
  MakeGAPDocDoc(Concatenation(PackageInfo("citrus")[1]!.
   InstallationPath, "/doc"), "citrus.xml", 
   ["utils.xml", "greens.xml", "orbits.xml", "properties.xml",
     "transform.xml", "../PackageInfo.g"], "citrus", "MathJax");;
end);

# new for 0.4! - CitrusMathJaxDefault - "for no argument"
#############################################################################

InstallGlobalFunction(CitrusMathJaxDefault, 
function()
GAPDoc2HTMLProcs.Head1MathJax:=Concatenation(
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n\n<!DOCTYPE html PUBLIC",
"\"-//W3C//DTD \ XHTML 1.0 Strict//EN\"\n",
"\"http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dt\"",
"d\">\n\n<html xmlns=\"http://www.w3.org/1999/xhtml\"",
"xml:lang=\"en\">\n<head>\n<script",
" type=\"text/javascript\"\n",
"src=\"http://cdn.mathjax.org/mathjax/latest/MathJax",
".js?config=TeX-AMS-MML_HTMLorMML\">\n</script>\n<title>GAP (");
Info(InfoWarning, 1, "don't forget to run CitrusMakeDoc()");
end);

# new for 0.4! - CitrusMathJaxLocal - "for a path to the MathJax folder"
#############################################################################

InstallGlobalFunction(CitrusMathJaxLocal, 
function(arg)
  local path;

  if Length(arg)>0 then 
    path:= arg[1];
  else
    path:= "";
  fi;

  GAPDoc2HTMLProcs.Head1MathJax:=Concatenation(
  "<?xml version=\"1.0\"",
  "encoding=\"UTF-8\"?>\n\n<!DOCTYPE html PUBLIC \"-//W3C/\"",
  "/DTD XHTML 1.0 Strict//EN\"\n\"http://www.w3.org/TR/xhtml1/DTD/xhtml1\"",
  "-strict.dtd\">\n\n<html xmlns=\"http://www.w3.org/1999/xhtml\"",
  "xml:lang=\"en\"\ >\n<head>\n<script type=\"text/javascript\"",
  "\n src=\"", path, "/MathJax/MathJax.js?config=default",
  "\">\n</script>\n<title>GAP\ (");
  Info(InfoWarning, 1, "don't forget to run CitrusMakeDoc()");
end);

# mod for 0.4! - CitrusTestAll - "for no argument"
#############################################################################

InstallGlobalFunction(CitrusTestAll, 
function()
  Print(
  "Reading all .tst files in the directory citrus/tst/...\n\n"); 
  Read(Filename(DirectoriesPackageLibrary("citrus","tst"),"testall.g"));;
end);

# new for 0.1! - CitrusTestInstall - "for no argument"
#############################################################################

InstallGlobalFunction(CitrusTestInstall, 
function()
  ReadTest(Filename(DirectoriesPackageLibrary("citrus","tst"),
   "testinstall.tst"));;
end);

# new for 0.1! - CitrusTestManualExamples - "for no argument"
#############################################################################

InstallGlobalFunction(CitrusTestManualExamples,
function()
  local InfoLevelInfoWarning, InfoLevelInfoCitrus;
  SizeScreen([80]); 
  InfoLevelInfoWarning:=InfoLevel(InfoWarning);;
  InfoLevelInfoCitrus:=InfoLevel(InfoCitrus);;
  SetInfoLevel(InfoWarning, 0);;
  SetInfoLevel(InfoCitrus, 0);

  TestManualExamples(Concatenation(PackageInfo("citrus")[1]!.
     InstallationPath, "/doc"), "citrus.xml", 
     ["utils.xml", "greens.xml", "orbits.xml", "properties.xml",
      "transform.xml", "../PackageInfo.g"]);
  
  SetInfoLevel(InfoWarning, InfoLevelInfoWarning);;
  SetInfoLevel(InfoCitrus, InfoLevelInfoCitrus);;
  Unbind(InfoLevelInfoCitrus);; Unbind(InfoLevelInfoWarning);;

end);

# new for 0.1! - DClass - "for a trans. semi and trans. or Green's class"
#############################################################################
# Usage: (trans. semigp. and trans.) or H-class or L-class or R-class.

InstallGlobalFunction(DClass, 
function(arg)

  if Length(arg)=2 and IsTransformationSemigroup(arg[1]) 
   and IsTransformation(arg[2]) then 
    return GreensDClassOfElement(arg[1], arg[2]);
  elif Length(arg)=1 and IsGreensRClass(arg[1]) then 
    return DClassOfRClass(arg[1]);
  elif Length(arg)=1 and IsGreensLClass(arg[1]) then 
    return DClassOfLClass(arg[1]);
  elif Length(arg)=1 and IsGreensHClass(arg[1]) then 
    return DClassOfHClass(arg[1]);
  fi;
  
  Info(InfoWarning, 1, "Usage: (trans. semigp. and trans.) or H-class or",
  " L-class or R-class.");
  return fail;
end);

# new for 0.1! - DClassNC - "for a trans. semi. and trans."
#############################################################################
# Usage: trans. semigp. and trans.

InstallGlobalFunction(DClassNC,
function(arg)

  if Length(arg)=2 and IsTransformationSemigroup(arg[1])
   and IsTransformation(arg[2]) then
    return GreensDClassOfElementNC(arg[1], arg[2]);
  fi;

  Info(InfoWarning, 1, "Usage: trans. semigp. and trans.");
  return fail;
end);

# new for 0.1! - Degree - "for a transformation"
#############################################################################
# Notes: returns DegreeOfTransformation.

InstallOtherMethod(Degree, "for a transformation",
[IsTransformation], f-> DegreeOfTransformation(f));

# new for 0.1! - Degree - "for a transformation semigroup"
#############################################################################
# Notes: returns DegreeOfTransformationSemigroup.

InstallOtherMethod(Degree, "for a transformation semigroup",
[IsTransformationSemigroup], s-> DegreeOfTransformationSemigroup(s));

# new for 0.1! - Generators - "for a semigroup or monoid"
############################################################################
# Notes: returns the monoid generators of a monoid, and the semigroup 
# generators of a semigroup. 

InstallOtherMethod(Generators, "for a semigroup or monoid",
[IsSemigroup],
function(s)

  if IsMonoid(s) then
    return GeneratorsOfMonoid(s);
  fi;

  return GeneratorsOfSemigroup(s);
end);

# new for 0.1! - HClass - "for a trans. semi. and trans."
#############################################################################
# Usage: trans. semigp. and trans.

InstallGlobalFunction(HClass, 
function(arg)

  if Length(arg)=2 and IsTransformationSemigroup(arg[1]) 
   and IsTransformation(arg[2]) then 
    return GreensHClassOfElement(arg[1], arg[2]);
  fi;

  Info(InfoWarning, 1, "Usage: trans. semigp. and trans.");
  return fail;
end);

# new for 0.1! - HClassNC - "for a trans. semi. and trans."
#############################################################################
# Usage: trans. semigp. and trans.

InstallGlobalFunction(HClassNC, 
function(arg)

  if Length(arg)=2 and IsTransformationSemigroup(arg[1]) 
   and IsTransformation(arg[2]) then 
    return GreensHClassOfElementNC(arg[1], arg[2]);
  fi;

  Info(InfoWarning, 1, "Usage: trans. semigp. and trans.");
  return fail;
end);

# new for 0.1! - LClass - "for a trans. semi. and trans. or H-class"
#############################################################################
# Usage: (trans. semigp. and trans.) or H-class.

InstallGlobalFunction(LClass, 
function(arg)

  if Length(arg)=2 and IsTransformationSemigroup(arg[1]) 
   and IsTransformation(arg[2]) then 
    return GreensLClassOfElement(arg[1], arg[2]);
  elif Length(arg)=1 and IsGreensHClass(arg[1]) then 
    return LClassOfHClass(arg[1]);
  fi;
  
  Info(InfoWarning, 1, "Usage: (trans. semigp. and trans.) or H-class.");
  return fail;
end);

# new for 0.1! - LClassNC - "for a trans. semi. and trans."
#############################################################################
# Usage: trans. semigp. and trans.

InstallGlobalFunction(LClassNC, 
function(arg)

  if Length(arg)=2 and IsTransformationSemigroup(arg[1]) 
   and IsTransformation(arg[2]) then 
    return GreensLClassOfElementNC(arg[1], arg[2]);
  fi;
  
  Info(InfoWarning, 1, "Usage: trans. semigp. and trans.");
  return fail;
end);

# new for 0.1! - RandomTransformationSemigroup 
#############################################################################

InstallGlobalFunction(RandomTransformationSemigroup,
function(m,n)
  return Semigroup(Set(List([1..m], x-> RandomTransformation(n))));
end);

# new for 0.1! - RandomTransformationSemigroup 
###########################################################################
InstallGlobalFunction(RandomTransformationMonoid,
function(m,n)
  return Monoid(Set(List([1..m], x-> RandomTransformation(n))));
end);

# new for 0.1! - Rank - "for a transformation"
#############################################################################
# Notes: returns RankOfTransformation. 

InstallOtherMethod(Rank, "for a transformation",
[IsTransformation], f-> RankOfTransformation(f));

# new for 0.1! - RClass - "for a trans. semi. and trans. or H-class"
#############################################################################
# Usage: (trans. semigp. and trans.) or H-class.

InstallGlobalFunction(RClass, 
function(arg)

  if Length(arg)=2 and IsTransformationSemigroup(arg[1]) 
   and IsTransformation(arg[2]) then 
    return GreensRClassOfElement(arg[1], arg[2]);
  elif Length(arg)=1 and IsGreensHClass(arg[1]) then 
    return RClassOfHClass(arg[1]);
  fi;
  
  Info(InfoWarning, 1, "Usage: (trans. semigp. and trans.) or H-class.");
  return fail;
end);

# new for 0.1! - RClassNC - "for a trans. semi. and trans."
#############################################################################
# Usage: trans. semigp. and trans.

InstallGlobalFunction(RClassNC, 
function(arg)

  if Length(arg)=2 and IsTransformationSemigroup(arg[1]) 
   and IsTransformation(arg[2]) then 
    return GreensRClassOfElementNC(arg[1], arg[2]);
  fi;
  
  Info(InfoWarning, 1, "Usage: trans. semigp. and trans.");
  return fail;
end);

# new for 0.5! - ReadCitrus - "for a string and optional pos. int."
#############################################################################

InstallGlobalFunction(ReadCitrus, 
function(arg)
  local read_line, file, i, line;
  
  if not IsString(arg[1]) then 
    Error("first argument must be a string");
  fi;
  
  #  if not IsReadableFile(arg[1]) then 
  #    Error(arg[1], " is not a readable file");
  #  fi; #JDM for some reason this doesn't work...
  
  #JDM add check that file is formatted ok!

  #####

  read_line:=function(line)
    local m, n, r, dom, out, f, i, j;
    
    m:=Int([line[1]]);            # block size <10
    n:=Int(line{[2..m+1]});       # degree
    r:=(Length(line)-(m+1))/(m*n);# number of generators 
    dom:=[m+2..m*n+m+1]; out:=EmptyPlist(r);

    for i in [1..r] do
      out[i]:=EmptyPlist(n); 
      f:=line{dom+m*(i-1)*n};
      for j in [1..n] do 
        Add(out[i], Int(NormalizedWhitespace(f{[(j-1)*m+1..j*m]})));
      od;
      out[i]:=TransformationNC(out[i]);
    od;
    return out;
  end;

  #####

  file:=SplitString(StringFile(arg[1]), "\n"); #quicker than using a stream...

  if Length(arg)>1  then 
    if IsPosInt(arg[2]) then 
      if arg[2]<=Length(file) then 
        return read_line(file[arg[2]]);
      else
        Error(arg[1], " only has ", Length(file), " lines");
        return fail;
      fi;
    else
      Error("the second argument should be a positive integer");
      return fail;
    fi;
  fi;
  
  return List(file, read_line);
end);

# new for 0.5! - WriteCitrus - "for a trans. coll. and filename as string"
#############################################################################

# Usage: transformation collection and filename as a string.

# Returns: a string. 

InstallGlobalFunction(WriteCitrus, 
function(arg)
  local trans, gens, i, convert, output, n, m, str, s, f;
  
  if not Length(arg)=2 then 
    Error("Usage: trans. semigroup, trans. collection or list of same", 
    "and filename as string");
    return fail;
  fi;

  if IsExistingFile(arg[2]) and not IsWritableFile(arg[2]) then 
    Error(arg[2], " exists and is not a writable file");
    return fail;
  fi;

  if IsTransformationCollection(arg[1]) then 
    trans:=[arg[1]];
  elif IsTransformationCollection(arg[1][1]) then 
    trans:=arg[1];
  else
    Error("Usage: first arg must be trans. semi., trans. coll. or list of same");
    return fail;
  fi;

  gens:=EmptyPlist(Length(trans));
  for i in [1..Length(trans)] do 
    if IsTransformationSemigroup(trans[i]) then 
      if HasMinimalGeneratingSet(trans[i]) then
        gens[i]:=MinimalGeneratingSet(trans[i]);
      elif HasSmallGeneratingSet(trans[i]) then 
        gens[i]:=SmallGeneratingSet(trans[i]);
      else
        gens[i]:=Generators(trans[i]);
      fi;
    else
      gens:=trans;
    fi;
  od;
 
  #####

  convert:=function(f, m)
    local str, i;
    
    str:="";
    for i in f![1] do 
      i:=String(i);
      Append(str, Concatenation([ListWithIdenticalEntries(m-Length(i), ' ')],
      [i]));
    od;

    return Concatenation(str);
  end;

  #####

  output := OutputTextFile( arg[2], true );
  SetPrintFormattingStatus(output, false);
  
  for s in gens do 

    n:=String(DegreeOfTransformationCollection(s));
    m:=Length(n);
    str:=Concatenation(String(m), n);
  
    for f in s do
      Append(str, convert(f, m));
    od;

    AppendTo( output, str, "\n" );
  od;
  
  CloseStream(output);

  return;
end);

#EOF
