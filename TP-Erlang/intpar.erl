-module(intpar).
-export([integral/3, compute_interval_childs/7, compute_interval/6]).

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

%%función que ejecutan los hijos y "subhijos"
compute_interval_childs (F, A, B, Fa, Fb, Area, Pid) ->
                                M = (A+B)/2,
                                Fm = F(M),
                                Left_area = (Fa+Fm)*(M-A)/2,
                                Right_area = (Fm+Fb)*(B-M)/2,

                                Cmp_1=abs( Area - (Left_area + Right_area)),
				Cmp_2=math:pow(1.0, math:exp(-9)),
				Pid_child = self(),
				io:format("HIjo pid: ~w~n", [Pid_child]),
                                if
                                   Cmp_1 < Cmp_2  ->   Pid ! Left_area+Right_area;

                                true ->
                                        spawn(intpar, compute_interval_childs, [F, A, M, Fa, Fm, Left_area, Pid_child]),
                                        Right = compute_interval_childs(F, M, B, Fm, Fb, Right_area, Pid_child),
					receive Ihijo when is_float(Ihijo) ->
						Left = Ihijo
					end,                                        
					Pid ! Left+Right
                                end.

%%función que ejecuta el MASTER
compute_interval (F, A, B, Fa, Fb, Area) ->
                                M = (A+B)/2,
                                Fm = F(M),
                                Left_area = (Fa+Fm)*(M-A)/2,
                                Right_area = (Fm+Fb)*(B-M)/2,

                                Cmp_1=abs( Area - (Left_area + Right_area)),
				Cmp_2=math:pow(1.0, math:exp(-9)),
				Self=self(),
				io:format("Padre pid: ~w~n", [Self]),
                                if
                                   Cmp_1 < Cmp_2 -> Left_area+Right_area;
                                true ->
						%%Si soy la tarea padre, creo mi tarea hija, y sigo calculando
						spawn(intpar, compute_interval_childs, [F,A,M,Fa,Fm,Left_area,Self]),
						Right = compute_interval (F,M,B,Fm,Fb,Right_area),
						%%cuando el otro proceso termine de calcular y me devuelva el subintervalo ya calculado
						receive I when is_float(I) -> 
							%%YO proceso master, lo guardo en Left
					                                Left = I
						end,
						%%YO proceso master, sumo los subintervalos ya calculados, y devuelvo la integral ya calculada
						Left+Right
                                end.
