-module(intseq).
-export([integral/2, compute_interval/5]).

%% función x^2 para PROBAR
f(x) -> math:pow(x, 2).

%%---------------------------------------------------------
%% Función: integral/3
%% Calcula la integral definida entre A y B de la función F
%% Argumentos:
%% F: La función a integrar
%% A, B: Los extremos del intervalo de integración
%% Devuelve: El valor de la integral
%%---------------------------------------------------------

integral (A,B) ->
                                fa = f(A),
                                fb = f(B),
                                area = (fa+fb)*(B-A)/2,
                                integral = compute_interval(A,B,fa,fb,area),
                                io:format("Integral de x^2: ~p~n",[integral]).

compute_interval (A, B, fa, fb, area) -> 
                                m = (a+b)/2,
                                fm = f(m),
                                left_area = (fa+fm)*(m-a)/2,
                                right_area = (fm+fb)*(b-m)/2,

                                if
                                   abs( area- (left_area+ right_area)) < math:pow(1.0,exp(-9) )  ->   ci = left_area+right_area;
                                                                                                                                         ci;

                                true ->
                                        left = compute_interval (a,m,fa,fm,left_area);
                                        right = compute_interval (m,b,fm,fb,right_area);
                                        cid = left_area+right_area;
                                        cid;
                                end.
