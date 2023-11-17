import 'package:can_to_go_gui/presentation/pages/can_trace_tab.dart';
import 'package:flutter/material.dart';

import 'package:can_to_go_gui/presentation/pages/global_app_bar.dart';
import 'package:can_to_go_gui/presentation/pages/can_overview_tab.dart';

class HomePage extends StatelessWidget {
  HomePage({Key? key}) : super(key: key);

  static const List<String> appBarTaps = ['Overview', 'Trace'];
  final List<Tab> _tabs = appBarTaps.map((tab) => Tab(text: tab)).toList();

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
        title: "CAN TO GO SYSTEM",
        home: DefaultTabController(
          length: appBarTaps.length,
          child: Scaffold(
            appBar: PreferredSize(
              preferredSize: const Size.fromHeight(100.0),
              child: GlobalAppBar(
                tabs: _tabs,
              ),
            ),
            body: TabBarView(
              children: [
                CanOverviewTab(),
                CanTraceTab(),
              ],
            ),
          ),
        ));
  }
}
