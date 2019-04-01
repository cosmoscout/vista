%
% VistaStyleguides.cls  
%

\NeedsTeXFormat{LaTeX2e}
\ProvidesClass{VistaStyleguides}[2006/09/18 RWTH Aachen University]

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% declare and process options 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\newif\if@litnum     \@litnumfalse
\newif\if@indexnum   \@indexnumfalse
\newif\if@VRThesisgerman

\DeclareOption{litnum}{\@litnumtrue}
\DeclareOption{indexnum}{\@indexnumnumtrue}
\DeclareOption{german}{%
    \@VRThesisgermantrue
}
\DeclareOption{sserif}{%
		\renewcommand{\familydefault}{\sfdefault}
}
% pass unknown options to class 'scrbook' 
\DeclareOption*{\PassOptionsToClass{\CurrentOption}{scrbook}}

% process options
% clsguide: ".. because \ProcessOptions has a *-form, it is wise to
%            follow the non-star form with \relax [...] since this 
%            prevents unncessary look ahead and possibly misleading 
%            error messages being issued."
\ProcessOptions\relax

\PassOptionsToClass{fleqn,12pt,a4paper,titlepage}{scrbook}
\LoadClass{scrbook}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% load required packages
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\RequirePackage[fleqn]{amsmath}     % math-symbols
\RequirePackage{amssymb} 				    % math-symbols
\RequirePackage{ifthen}             % enable if .. then .. else structures in
                                    % class-file
\RequirePackage{calc}               % enable primitive calculations in class
\RequirePackage[scaled=.92]{helvet}
\RequirePackage{courier}
\RequirePackage{array}              % better table support
\RequirePackage{longtable}          % support for long tables
\RequirePackage[dvips]{graphicx}    % support eps-files
\RequirePackage{fancyhdr}           % support nice page-headers
\RequirePackage{hangcaption}
%\RequirePackage[ps2pdf]{hyperref}
\RequirePackage{makeidx}            % support index
%\RequirePackage[small,hang]{caption} % modifie default caption layout
\RequirePackage{caption}[2003/12/20] %enforce most current version of caption
\RequirePackage{varwidth}
\RequirePackage{color}
\RequirePackage{psfrag}
\RequirePackage{amsthm}            % math-theorems

%\RequirePackage{verbatim}
\RequirePackage{listings}
%\RequirePackage{footnote}
%\RequirePackage{picins}

\RequirePackage{fancyref}


%%%%%%%%%%%%%%%%%%%%%%%
% Bilder
\newcounter{radius}
\newcounter{diameter}
\newcounter{linestart}
\newcounter{lineend}

\newcommand{\verttrans}[2]{
  \setcounter{diameter}{#1}
  \setcounter{radius}{\value{diameter}/2}
  \setcounter{lineend}{#2 - 3*\value{radius}}
  \begin{picture}(#1,#2)
    \path(\value{radius},\value{lineend})(\value{radius},\value{diameter})
    \setcounter{lineend}{\value{lineend} + \value{radius}} 
    \put(\value{radius},\value{lineend}){\ellipse{\value{diameter}}{\value{diameter}}}
    \put(\value{radius},\value{radius}){\blacken\ellipse{\value{diameter}}{\value{diameter}}}
    \put(\value{radius},\value{radius}){\ellipse{\value{diameter}}{\value{diameter}}}
  \end{picture}
}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Kopfzeile und Fußzeilen
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Logoeinstellungen:
% Groesse :height=?em
% Position vertikal: \raisbox{?em}
% Position horizontal: \hspace{?em}

\newcommand{\@vrfootlogo}{%
  \raisebox{.15em}[0em]{\includegraphics[height=.8em]{cc-by-sa}}%
}

\newcommand{\foottitle}{VR Group RWTH Aachen}

\newcommand{\@vrfoottext}{\raisebox{0em}[0cm]{%
    \raisebox{.15em}[0em]{\textsc\small \foottitle}}}
\newcommand{\@rwthfootlogo}{
  \raisebox{.15em}[0em]{\includegraphics[height=.8em]{RWTHlogo}}%
}
\newcommand{\@rightlogo}{\@rwthfootlogo}
\newcommand{\@leftlogo}{\@vrfootlogo}

\lhead[\sffamily \selectfont \textsc{\leftmark}]{}
\rhead[]{\sffamily \selectfont \textsc{\rightmark}}

\if@twoside
    \cfoot{}
    \lfoot[\thepage]{\@rightlogo}
    \rfoot[\@leftlogo]{\thepage}
\else
    \cfoot{\thepage}
    \lfoot[\@rightlogo]{\@leftlogo}
    \rfoot[\@leftlogo]{\@rightlogo}
    
\fi

\fancypagestyle{plain}{
  \fancyhead{}
  \fancyfoot{}
  \if@twoside
      \fancyfoot[LO]{{\@rightlogo}}
      \fancyfoot[RO]{{\thepage}}
  \else
      \fancyfoot[L]{{\@leftlogo}}
  \fi
  \renewcommand{\headrulewidth}{0pt}
}

\pagestyle{fancy}

%\renewcommand{\chaptermark}[1]{%
%  \markboth {%
%    \ifnum \value{secnumdepth} >\m@ne
%      \thechapter\ \ %
%    \fi%
%    #1}{%
%    \ifnum \value{secnumdepth} >\m@ne
%      \thechapter\hspace{.8em}%
%    \fi%
%    #1}
%}
%
%\newcommand{\schaptermark}[1]{%
%  \markboth {%
%    #1}{%
%    #1}
%}
%
%\renewcommand{\sectionmark}[1]{%
%  \markright {%
%    \ifnum \value{secnumdepth} >\z@%
%    \thesection\hspace{.7em}%
%    \fi%
%    #1%
%  }
%}%
%\renewcommand{\subsectionmark}[1]{}

%\renewcommand\tableofcontents{%
%  \chapter*{\contentsname}
%  \addtocontents{toc}{\protect\contentsline{chapter}{\contentsname}{\thepage}{}}
%  \markboth{\contentsname}{\contentsname}%
%  \@starttoc{toc}%
%}

\setcounter{secnumdepth}{4}
\setcounter{tocdepth}{3}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Formelnummern 
%% * Nummerierung der Formeln mit Kapitelnummer
%%   kombinieren
%% * Zähler equation bei neuem Kapitel zurücksetzen
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\renewcommand{\theequation}{\thechapter.\arabic{equation}}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Index
%%   * Name: Stichwortregister
%%   * Eintrag ins Inhaltsverzeichnis
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%\renewenvironment{theindex}
%{
%  \if@twocolumn
%    \@restonecolfalse
%  \else
%    \@restonecoltrue
%  \fi
%  \columnseprule \z@
%  \columnsep 35\p@
%  \twocolumn
%  \if@indexnum {
%    \chapter{\indexname}
%  }
%  \else {
%    \chapter*{\indexname}
%    \schaptermark{\indexname}
%    \addtocontents{toc}{\protect\contentsline{chapter}{\indexname}{\thepage}{}}
%  }
%  \fi
%  \parindent\z@
%  \parskip\z@ \@plus .3\p@\relax
%  \let\item\@idxitem
%}{
%  \if@restonecol\onecolumn\else\clearpage\fi%
%}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Literaturverzeichnis
%%   * Eintrag ins Inhaltsverzeichnis
%%   * Klein-/Großschreibung in mkboth beibehalten
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%\renewenvironment{thebibliography}[1]
%{
%  \if@litnum {
%    \chapter{\bibname}%    Mit Kapitelnummer
%  }
%  \else {
%    \chapter*{\bibname}%   Ohne Kapitelnummer
%%    \schaptermark{\bibname}
%    \addtocontents{toc}{\protect\contentsline{chapter}{\bibname}{\thepage}{}}
%  }
%  \fi
%  
%  \list{\@biblabel{\@arabic\c@enumiv}}%
%  {\settowidth\labelwidth{\@biblabel{#1}}%
%    \leftmargin\labelwidth
%    \advance\leftmargin\labelsep
%    \@openbib@code
%    \usecounter{enumiv}%
%    \let\p@enumiv\@empty
%    \renewcommand\theenumiv{\@arabic\c@enumiv}}%
%  \clubpenalty4000
%  \@clubpenalty \clubpenalty
%  \widowpenalty4000%
%  \sfcode`\.\@m}
%{\def\@noitemerr
%  {\@latex@warning{Empty `thebibliography' environment}}%
%  \endlist}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Paragraph-Befehl
%% * Zeilenwechsel nach Paragraphtitel
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%\renewcommand\section{\@startsection {section}{1}{\z@}%
%                                   {-3.5ex \@plus -1ex \@minus -.2ex}%
%                                   {2.3ex \@plus.2ex \@minus 1.9ex}%
%                                   {\sffamily\large\bfseries}}
%\renewcommand\subsection{\@startsection{subsection}{2}{\z@}%
%                                     {-3.25ex\@plus -1ex \@minus -.2ex}%
%                                     {1.5ex \@plus .2ex \@minus 1.0ex}%
%                                     {\sffamily\bfseries}}
%\renewcommand\subsubsection{\@startsection{subsubsection}{3}{\z@}%
%                                     {-3.25ex\@plus -1ex \@minus -.2ex}%
%                                     {1.5ex \@plus .2ex \@minus 1.0ex}%
%                                     {\sffamily\bfseries}}
%\renewcommand\paragraph{\@startsection{paragraph}{4}{\z@}%
%                                    {3.25ex \@plus1ex \@minus.2ex}%
%                                    {.5em}%
%                                    {\sffamily\normalsize\bfseries}}

% don't count paragraphs
\setcounter{secnumdepth}{3}

%set caption font
\newcommand{\VRcaptionlabelfont}{\normalfont\normalsize\bfseries}
\newcommand{\VRcaptiontextfont}{\normalfont\normalsize\mdseries}

\long\def\@makecaption#1#2{%
  \vskip\abovecaptionskip
  \sbox\@tempboxa{\VRcaptionlabelfont #1: \VRcaptiontextfont #2}%
  \ifdim \wd\@tempboxa >\hsize
    {\VRcaptionlabelfont #1: \VRcaptiontextfont #2\par}
  \else
    \global \@minipagefalse
    \hb@xt@\hsize{\hfil\box\@tempboxa\hfil}%
  \fi
  \vskip\belowcaptionskip}
\if@twoside
 \@openrighttrue
\fi

\newlength{\@wchapnum}
\newlength{\@wchaptitle}
\newsavebox{\@chaptitle}

\newlength{\@maxtitlewidth}
\setlength{\@maxtitlewidth}{0.7\textwidth}

\newcommand{\@chaptitleformat}{\Large\bfseries}
\newcommand{\@linelift}{.3em}

\newlength{\@wquad}
\settowidth{\@wquad}{\quad}
\newcommand{\@leftfill}{\hrulefill}
\newcommand{\@rightfill}{\hrulefill}

\def\@makechapterhead#1{%
  \vspace*{10\p@}%
  {\parindent \z@ \raggedleft \reset@font
            \scshape \sffamily\fontsize{1.5em}{1.5em}\selectfont \@chapapp{} \thechapter
        \par\nobreak
        \interlinepenalty\@M
    \Huge \sffamily \bfseries #1\par\nobreak
    %\vspace*{1\p@}%
    \hrulefill
    \par\nobreak
    \vskip 100\p@
  }}
\def\@makeschapterhead#1{%
  \vspace*{10\p@}%
  {\parindent \z@ \raggedleft \reset@font
            \scshape \sffamily\fontsize{1.5em}{1.5em}\selectfont \vphantom{\@chapapp{} \thechapter}
        \par\nobreak
        \interlinepenalty\@M
    \Huge \sffamily \bfseries #1\par\nobreak
    %\vspace*{1\p@}%
    \hrulefill
    \par\nobreak
    \vskip 100\p@
  }}
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Neudefinition von \see und \seealso
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%\renewcommand{\see}[2]{\textit{\seename} #1}
%\providecommand{\seealso}[2]{}  % sicherstellen, dass der befehl
%                                % existiert. Alte Version von AMSmath
%                                % definiert dieses Commande nämlich nicht.
%\renewcommand{\seealso}[2]{\textit{\alsoname} #1}

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Längendefinitionen für Math-Umgebungen
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%\setlength{\arraycolsep}{5pt}  
%                                % Standard: 5pt
%                                % halbe Breite des horizontalen Spaltenzwischenraums,
%                                % der in einer aray-Umgebung definiert wird.
%
%\setlength{\jot}{4pt}          
%                                % Standard: 3pt
%                                % zusätzlicher vertikaler Zwischenraum bei 
%                                % eqnarray(*) zwischen den Zeilen
%\setlength{\mathindent}{2.5em} 
%                                % Standard: Einrückung einer Liste erster Ebene
%                                %           normalerweise 2.5em
%                                % Einzug vom Linken Rand bei Verwendung der 
%                                % Klassenoption fleqn
%
%% mit der Klassenoption fleqn haben die folgenden Längen keine Auswirkung
%% Es wird stattdessen immer  \topsep benutzt (wird weiter oben gesetzt)
%\setlength{\abovedisplayskip}{12pt plus 3pt minus 9pt} 
%                                % Standard: 12pt plus 3pt minus 9pt
%                                % Leerraum über langer Formel
%\setlength{\belowdisplayskip}{12pt plus 3pt minus 9pt} 
%                                % Standard: 12pt plus 3pt minus 9pt
%                                % Leerraum unter langer Formel
%\setlength{\abovedisplayshortskip}{0pt plus 3pt} 
%                                % Standard: 0pt plus 3pt
%                                % Leerraum über kurzer Formel
%\setlength{\belowdisplayshortskip}{7pt plus 3pt minus 9pt} 
%                                % Standard: 7pt plus 3pt minus 4pt
%                                % Leerraum unter kurzer Formel
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Neue Umgebung für Beweise
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
\theoremstyle{definition}
\newtheorem{beweis}{Beweis}
\renewenvironment{proof}{%
\begin{beweis}%
}{%
\qed%
\end{beweis}}%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Anpassung für Floatobjekte
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%\renewcommand{\floatpagefraction}{.9}
%\renewcommand{\textfraction}{.2}
%\renewcommand{\topfraction}{.57}
%\renewcommand{\bottomfraction}{.57}

\endinput

