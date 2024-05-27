tp_2a_s4_sources = [
    'main.c',
    'tp-2a-s4-application.c',
    'tp-2a-s4-window.c',
    'BinaryHeap.c',
    'FileParser.c',
    'Graph.c',
    'ListInt.c',
    'Logger.c',
    'MaxFlow.c',
    'Settings.h',
    'ShortestPath.c',
    'Timer.c',
    'TSPAlgorithm.c',
    'shumate-test-tile-source.c',
    'DataPipeline.c',
    'SpatialMap.c',
    'SearchTrie.c',
    'TextUtils.c',
    'Test.c',
    'SearchEngine.c',
    'CommandLine.c',
    'Evolution.c'
]

tp_2a_s4_deps = [
    dependency('gtk4'),
    dependency('libadwaita-1', version : '>= 1.4'),
    dependency('shumate-1.0')
]

tp_2a_s4_sources += gnome.compile_resources('tp-2a-s4-resources',
                                            'tp-2a-s4.gresource.xml',
                                            c_name : 'tp_2a_s4'
)

executable('tp-2a-s4', tp_2a_s4_sources,
           link_args : ['-lm'],
           dependencies : tp_2a_s4_deps,
           install : true,
)
