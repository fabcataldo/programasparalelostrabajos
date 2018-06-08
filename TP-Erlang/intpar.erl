-module(intpar).
-export([integral/3, compute_interval/6]).

%%---------------------------------------------------------
%% Función: integral/3
%% Calcula la integral definida entre A y B de la función F
%% Argumentos:
%% F: La función a integrar
%% A, B: Los extremos del intervalo de integración
%% Devuelve: El valor de la integral
%%---------------------------------------------------------

integral (F, A,B) ->
                                Fa = F(A),
                                Fb = F(B),
                                Area = (Fa+Fb)*(B-A)/2,
                                Integral = compute_interval(F, A,B,Fa,Fb,Area),
                                io:format("Integral de x^2: ~p~n",[Integral]).

compute_interval (F, A, B, Fa, Fb, Area) -> 
                                M = (A+B)/2,
                                Fm = F(M),
                                Left_area = (Fa+Fm)*(M-A)/2,
                                Right_area = (Fm+Fb)*(B-M)/2,

                                Cmp_1=abs( Area - (Left_area + Right_area)),
				Cmp_2=math:pow(1.0, math:exp(-9)),
                                if
                                   Cmp_1 < Cmp_2  ->   Left_area+Right_area;

                                true ->
					%%creo un nuevo proceso, diciendolé que calcule uno de los subintervalos
					pid=spawn(intseq, compute_interval, [F,A,M,Fa,Fm,Left_area]),
					%%YO, proceso master,sigo calculando el otro subintervalo
                                        Right = compute_interval (F,M,B,Fm,Fb,Right_area),
					%%cuando el otro proceso termine de calcular y me devuelva el subintervalo ya calculado
                                        receive I when is_float(I) -> 
											%%YO proceso master, lo guardo en Left
                                                                                           Left = I
                                        end,
					%%YO proceso master, sumo los subintervalos ya calculados, y devuelvo la integral ya calculada
                                        Left+Right
                                end.
