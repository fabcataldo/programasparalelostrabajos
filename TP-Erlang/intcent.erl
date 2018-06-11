-module(intcent).
-export([integral/3, compute_interval_childs/9, master/3]).

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
				Pid_master=self(),
                                Integral = master(0, 0, {F, A,B,Fa,Fb,Area, Pid_master}),
                                io:format("Integral de x^2: ~p~n",[Integral]).

%%función que ejecutan los hijos y "subhijos"
compute_interval_childs (F, A, B, Fa, Fb, Area, Pid, Workers, Problems_New) ->
                                M = (A+B)/2,
                                Fm = F(M),
                                Left_area = (Fa+Fm)*(M-A)/2,
                                Right_area = (Fm+Fb)*(B-M)/2,

                                Cmp_1=abs( Area - (Left_area + Right_area)),
				Cmp_2=math:pow(1.0, math:exp(-9)),
				Pid_child = self(),
				Workers_2=Workers-1,
				Problems_New_2=Problems_New-1,
				Tmp=Left_area+Right_area,
				%%io:format("HIjo pid: ~w~n", [Pid_child]),
                                if
                                   Cmp_1 < Cmp_2  ->   %%si terminé, mando mi suma a quien me llamó, y llamo al master para decirle "che, ya terminé"
									Pid ! Tmp;
									master(Workers_2, Problems_New_2, {Pid});
                                true ->
					%%si no, mando uno de mis problemas al master, y calculo la otra mitad del área asignada yo
                                        master (Workers, Problems_New, {F, A, M, Fa, Fm, Left_area, Pid} ), %%Left
                                        compute_interval_childs(F, M, B, Fm, Fb, Right_area, Pid_child, Workers, Problems_New) %%Right
                                end.

%%Si Workers es 0 (cantidad de workers ocupados), no hay problemas para resolver, y devuelvo un resultado junto con Pid, mando a mis hijos "fin" y devuelvo a "integral()" la suma hasta el momento
master(Workers=0, Problems=0, {Pid}) ->
					    				receive R when is_float(R) ->
										Resultado=R
									end,    
									Pid ! fin,
									Resultado;

%%Si la cantidad de workers ocupados es distinto de 0 o igual a 0, y la cantidad de problemas a resolver es distinto de 0 o igual a 0 y estoy enviando argumentos para una tarea, creo una nueva tarea
master(Workers>=0, Problems>=0, {F, A, B, Fa, Fb, Area, Pid}) ->
															Problems_New=Problems+1,
															Workers_New=Workers+1,
															if
																(Workers_New =< 10) == true -> 
																			spawn(intcent, compute_interval_childs, [F, A, B, Fa, Fb, Area, Pid, Workers_New, Problems_New] )
															end.
