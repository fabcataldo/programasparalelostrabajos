-module(intseq).
-export([integral/2, compute_interval/5]).

%% función x^2 para PROBAR
f(X) -> math:pow(X, 2).

%%---------------------------------------------------------
%% Función: integral/3
%% Calcula la integral definida entre A y B de la función F
%% Argumentos:
%% F: La función a integrar
%% A, B: Los extremos del intervalo de integración
%% Devuelve: El valor de la integral
%%---------------------------------------------------------

integral (A,B) ->
                                Fa = f(A),
                                Fb = f(B),
                                Area = (Fa+Fb)*(B-A)/2,
                                Integral = compute_interval(A,B,Fa,Fb,Area),
                                io:format("Integral de x^2: ~p~n",[Integral]).

compute_interval (A, B, Fa, Fb, Area) -> 
                                M = (A+B)/2,
                                Fm = f(M),
                                Left_area = (Fa+Fm)*(M-A)/2,
                                Right_area = (Fm+Fb)*(B-M)/2,

                                Cmp_1=abs( Area - (Left_area + Right_area)),
				Cmp_2=math:pow(1.0, math:exp(-9)),
                                if
                                   Cmp_1 < Cmp_2  ->   Left_area+Right_area;

                                true ->
                                        Left = compute_interval (A,M,Fa,Fm,Left_area),
                                        Right = compute_interval (M,B,Fm,Fb,Right_area),
                                        Left+Right
                                end.
