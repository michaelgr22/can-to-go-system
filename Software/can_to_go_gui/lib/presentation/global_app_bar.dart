import 'package:flutter/material.dart';

class GlobalAppBar extends StatelessWidget {
  final List<Tab> tabs;

  GlobalAppBar({required this.tabs});

  static const String _title = 'CAN TO GO SYSTEM';

  @override
  Widget build(BuildContext context) {
    return AppBar(
      bottom: TabBar(
        tabs: tabs,
        indicatorColor: Colors.blue,
        labelColor: Colors.blue,
        labelStyle: const TextStyle(
          fontWeight: FontWeight.bold,
        ),
      ),
      backgroundColor: Colors.black38,
      title: const Text(
        _title,
        style: TextStyle(
          color: Colors.blue,
          fontWeight: FontWeight.bold,
        ),
      ),
      centerTitle: true,
    );
  }
}
